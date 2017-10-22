
#include "extension_test_runner.h"

#include <string>
#include <sstream>
#include <iostream>
#if defined(__APPLE__) || defined(__linux__)
#include <signal.h>
#include <unistd.h>
#endif
#include "text_annotater.h"
#include "diagnostics.h"
#include "extension.h"
#include "utils.h"
#include "Core.h"

using namespace Lya::TestFramework;
using namespace Lya::Utils;
using namespace Lya::Types;

namespace Lya::Extension {

	int child;
	void kill_all_processes(int signum) {
	#if defined(__APPLE__) || defined(__linux__)
	    kill(child, SIGTERM);
	#endif
	}

	ExtensionTestRunner::ExtensionTestRunner(shared_ptr<Session> _session):
		session(_session) { }

	string ExtensionTestRunner::get_file_to_localization_json_string(const FileToLocalizations& file_to_localizations) {
		Json::Value result;
		for (const auto& fl : file_to_localizations) {
			auto localizations = fl.second;
			Json::Value localizations_json = Json::arrayValue;
			for (const auto& l : fl.second) {
				Json::Value params_json = Json::arrayValue;
				for (const auto& p : l.params) {
					Json::Value param_json;
					param_json["name"] = p.name;
					if (*p.type == "") {
						param_json["type"] = Json::nullValue;
					}
					else {
						param_json["type"] = *p.type;
					}
					param_json["is_list"] = p.is_list;
					params_json.append(param_json);
				}
				Json::Value localization_json;
				localization_json["id"] = l.id;
				localization_json["parameters"] = params_json;
				localizations_json.append(localization_json);
			}
			Json::Value localization_json = Json::arrayValue;
			result[fl.first] = localizations_json;
		}
		Json::StreamWriterBuilder builder;
		builder["indentation"] = "    ";
		unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		ostringstream string_buffer;
		writer->write(result, &string_buffer);
		return string_buffer.str();
	}

	void ExtensionTestRunner::check_error_file(const string& test_name, const string& test_file, const vector<Diagnostic>& diagnostics) {
		string current_error_file_path = current_test_file + ".errors.txt";

		TextAnnotater annotater(read_file(test_file));
		for (const auto& d : diagnostics) {
			auto l = d.location;
			annotater.annotate(l, d.message);
		}
		string current_string = annotater.get_text();
		write_file(current_error_file_path, current_string);
		string reference_string = "";
		string reference_file_path = replace_string(current_error_file_path, "currents", "references");
		if (file_exists(reference_file_path)) {
			reference_string = read_file(reference_file_path);
		}

		test(test_name + " Errors", [reference_string, current_string](Test* t) {
			if (current_string != reference_string) {
				throw runtime_error("Assertion Error!");
			}
		});
	}

	void ExtensionTestRunner::check_localization_file(const string& test_name, const FileToLocalizations& file_to_localizations) {
		string file_to_localization_json_string = get_file_to_localization_json_string(file_to_localizations);
		file_to_localization_json_string = replace_string(file_to_localization_json_string, session->root_dir + "tests/cases/", "");
		write_file(current_localization_file, file_to_localization_json_string);
		string reference_file_path = replace_string(current_localization_file, "currents", "references");
		string reference_string = "";
		if (file_exists(reference_file_path)) {
			reference_string = read_file(reference_file_path);
		}
		test(test_name + " Localizations", [reference_string, file_to_localization_json_string](Test* t) {
			if (file_to_localization_json_string != reference_string) {
				throw runtime_error("Assertion Error!");
			}
		});
	}

	void ExtensionTestRunner::run_extension_tests() {
	    string extension_file = join_paths(session->root_dir, "extension.json");
	    Extension* extension = Extension::create(session, extension_file);

	    const auto start_extension_server = [&]() -> void {
	        child = extension->start_server();
	        signal(SIGINT, kill_all_processes);
	    };

	    const auto for_each_key_extraction_test_file = [&](std::function<void (const string&)> callback) -> void {
	        vector<string> compilation_test_files = find_files(session->root_dir + "tests/cases/KeyExtractions/*");
	        for (auto const& f : compilation_test_files) {
	            callback(f);
	        }
	    };

	    if (session->start_server) {
	        start_extension_server();
	    }
	    while (!extension->is_available());
	    domain("KeyExtractions");
		remove_dir(session->root_dir + "tests/currents/KeyExtractions");
	    for_each_key_extraction_test_file([&](const string& test_file) {
		    current_test_file = replace_string(test_file, "cases", "currents");
		    current_test_file = replace_string(current_test_file, ".javascript", "");
		    current_localization_file = current_test_file + ".localization.json";
		    string currents_dir = current_test_file.substr(0, current_localization_file.find_last_of("/"));
		    string test_name = current_test_file.substr(current_test_file.find_last_of("/") + 1);
		    test_name = replace_string(test_name, session->root_dir, "");
		    test_name = replace_string(test_name, ".json", "");
		    if (session->test != nullptr && *session->test != test_name) {
			    return;
		    }
		    recursively_create_dir(currents_dir);

	        vector<string> files = { test_file };
	        tuple<FileToLocalizations, vector<Diagnostic>> result = extension->get_localizations(files, session->start_line);
			FileToLocalizations file_to_localizations = get<0>(result);
		    vector<Diagnostic> diagnostics = get<1>(result);
		    if (diagnostics.size() > 0) {
				check_error_file(test_name, test_file, diagnostics);
		    }
		    else {
			    check_localization_file(test_name, file_to_localizations);
		    }
	    });
	    run_tests();
	    print_result();
	    kill_all_processes(SIGTERM);
	}

} // Lya::Extension
