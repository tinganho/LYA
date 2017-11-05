
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
#include "definitions.h"


namespace Lya::extension {

	using namespace test_framework;
	using namespace utils;
	using namespace types;

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

	void ExtensionTestRunner::check_error_file(const string& test_name, const string& canonical_test_file_path, const vector<Diagnostic>& diagnostics) {
		string current_error_file_path = canonical_test_file_path + ".errors.txt";

		TextAnnotater annotater(read_file(canonical_test_file_path));
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

		test(test_name + " - errors", [reference_string, current_string](Test* t) {
			if (current_string != reference_string) {
				throw runtime_error("Assertion Error!");
			}
		});
	}

	void ExtensionTestRunner::check_localization_file(const string& localization_file, const string& test_name, const FileToLocalizations& file_to_localizations) {
		string file_to_localization_json_string = get_file_to_localization_json_string(file_to_localizations);
		file_to_localization_json_string = replace_string(file_to_localization_json_string, session->root_dir + "tests/cases/", "");
		write_file(localization_file, file_to_localization_json_string);
		string reference_file_path = replace_string(localization_file, "currents", "references");
		string reference_string = "";
		if (file_exists(reference_file_path)) {
			reference_string = read_file(reference_file_path);
		}
		test(test_name + " - localizations", [reference_string, file_to_localization_json_string](Test* t) {
			if (file_to_localization_json_string != reference_string) {
				throw runtime_error("Assertion Error!");
			}
		});
	}

	void ExtensionTestRunner::visit_extraction_test(std::function<void(const string &)> callback) {
		vector<string> tests = find_files(session->root_dir + "tests/cases/extractions/*");
		for (auto const& t : tests) {
			callback(t);
		}
	}

	void ExtensionTestRunner::visit_compile_test(function<void(const string &)> callback) {
		vector<string> tests = find_files(session->root_dir + "tests/cases/compilations/*");
		for (auto const& t : tests) {
			callback(t);
		}
	}

	void ExtensionTestRunner::start_extension_server() {
		child = extension->start_server();
		signal(SIGINT, kill_all_processes);
	};

	void ExtensionTestRunner::run_extension_tests() {
	    string extension_file = resolve_paths(session->root_dir, "extension.json");
	    extension = make_shared<Extension>(Extension::create(session, extension_file));

	    if (session->start_server) {
	        start_extension_server();
	    }
	    while (!extension->is_available());
	    domain("KeyExtractions");
		remove_folder(session->root_dir + "tests/currents/extractions");
		define_tests();
	    run_tests();
	    print_result();
	    kill_all_processes(SIGTERM);
	}

	void ExtensionTestRunner::define_tests() {
		define_extraction_tests();
		define_compile_tests();
	}

	void ExtensionTestRunner::define_extraction_tests() {
		visit_extraction_test([&](const string &test_file_path) {
			string current_canonical_file_path = get_canonical_test_file_path(test_file_path);
			string test_name = current_canonical_file_path.substr(current_canonical_file_path.find_last_of("/") + 1);
			if (should_skip_test(test_name)) {
				return;
			}
			cout << current_canonical_file_path << folder_path(current_canonical_file_path) << endl;
			recursively_create_folder(folder_path(current_canonical_file_path));
			string localization_file = current_canonical_file_path + ".localization.json";

			tuple<FileToLocalizations, vector<Diagnostic>> result = extension->get_localizations(
					vector<string> {test_file_path}, session->start_line);
			FileToLocalizations file_to_localizations = get<0>(result);
			vector<Diagnostic> diagnostics = get<1>(result);
			if (diagnostics.size() > 0) {
				check_error_file(test_name, current_canonical_file_path, diagnostics);
			} else {
				check_localization_file(localization_file, test_name, file_to_localizations);
			}
		});
	}

	void ExtensionTestRunner::define_compile_tests() {
		visit_compile_test([&](const string &test_file_path) {
			string current_canonical_file_path = get_canonical_test_file_path(test_file_path);
			string test_name = current_canonical_file_path.substr(current_canonical_file_path.find_last_of("/") + 1);
			if (should_skip_test(test_name)) {
				return;
			}
			cout << current_canonical_file_path << folder_path(current_canonical_file_path) << endl;
			recursively_create_folder(folder_path(current_canonical_file_path));
		});
	}

	bool ExtensionTestRunner::should_skip_test(const string& test_name) {
		if (session->test != nullptr && *session->test != test_name) {
			return true;
		}
		return false;
	}

	string ExtensionTestRunner::get_canonical_test_file_path(const string &test_file) const {
		string file = replace_string(file, "cases", "currents");
		file = file.substr(0, file.find_last_of("."));
		return file;
	}

} // Lya::Extension
