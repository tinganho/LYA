
#include "ExtensionTestRunner.h"

#include <string>
#include <sstream>
#include <iostream>
#if defined(__APPLE__) || defined(__linux__)
#include <signal.h>
#include <unistd.h>
#endif
#include "Types.h"
#include "Utils.h"
#include "Diagnostics.h"
#include "Extension.h"
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
					param_json["type"] = *p.type;
					param_json["is_list"] = p.is_list;
					params_json.append(param_json);
				}
				Json::Value localization_json;
				localization_json["id"] = l.id;
				localization_json["params"] = params_json;
				localization_json["line"] = l.line;
				localization_json["column"] = l.column;
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

	void ExtensionTestRunner::check_error_file(const string& test_name, const vector<Diagnostic>& diagnostics) {
		for (const auto& d : diagnostics) {
			d.location.line;
			d.location.column;
		}

//		test(test_name, [reference_string, file_to_localization_json_string](Test* t) {
//			if (file_to_localization_json_string != reference_string) {
//				throw runtime_error("Assertion Error!");
//			}
//		});
	}

	void ExtensionTestRunner::check_localization_file(const string& test_name, const FileToLocalizations& file_to_localizations) {
		string file_to_localization_json_string = get_file_to_localization_json_string(file_to_localizations);
		file_to_localization_json_string = replace_string(file_to_localization_json_string, session->root_dir + "Tests/Cases/", "");
		write_file(replace_string(current_localization_file_path, "Cases", "Currents"), file_to_localization_json_string);
		string reference_file_path = replace_string(current_localization_file_path, "Currents", "References");
		string reference_string = "";
		if (file_exists(reference_file_path)) {
			reference_string = read_file(reference_file_path);
		}
		test(test_name, [reference_string, file_to_localization_json_string](Test* t) {
			if (file_to_localization_json_string != reference_string) {
				throw runtime_error("Assertion Error!");
			}
		});
	}

	void ExtensionTestRunner::run_extension_tests() {
	    string extension_file = join_paths(session->root_dir, "Extension.json");
	    Extension* extension = Extension::create(session, extension_file);

	    const auto start_extension_server = [&]() -> void {
	        child = extension->start_server();
	        signal(SIGINT, kill_all_processes);
	    };

	    const auto for_each_key_extraction_test_file = [&](std::function<void (const string&)> callback) -> void {
	        vector<string> compilation_test_files = find_files(session->root_dir + "Tests/Cases/KeyExtractions/*");
	        for (auto const& f : compilation_test_files) {
	            callback(f);
	        }
	    };

	    if (session->start_server) {
	        start_extension_server();
	    }
	    while (!extension->is_available());
	    domain("KeyExtractions");
		remove_dir(session->root_dir + "Tests/Currents/KeyExtractions");
	    for_each_key_extraction_test_file([&](const string& test_file) {
		    current_localization_file_path = replace_string(test_file, "Cases", "Currents");
		    current_localization_file_path = current_localization_file_path + ".localization.json";
		    string current_error_file_path = current_localization_file_path + ".errors.txt";
		    string currents_dir = current_localization_file_path.substr(0, current_localization_file_path.find_last_of("/"));
		    string test_name = current_localization_file_path.substr(current_localization_file_path.find_last_of("/") + 1);
		    test_name = replace_string(test_name, session->root_dir, "");
		    test_name = replace_string(test_name, ".json", "");
		    if (session->test != nullptr && *session->test != test_name) {
			    return;
		    }
		    recursively_create_dir(currents_dir);

	        vector<string> files = { test_file };
	        tuple<FileToLocalizations, vector<Diagnostic>> result = extension->get_localizations(files);
			FileToLocalizations file_to_localizations = get<0>(result);
		    vector<Diagnostic> diagnostics = get<1>(result);
		    if (diagnostics.size() > 0) {
				check_error_file(test_name, diagnostics);
		    }
		    check_localization_file(test_name, file_to_localizations);
	    });
	    run_tests();
	    print_result();
	    kill_all_processes(SIGTERM);
	}

} // Lya::Extension
