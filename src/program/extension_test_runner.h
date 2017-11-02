
#ifndef EXTENSION_TEST_RUNNER_H
#define EXTENSION_TEST_RUNNER_H

#include "types.h"
#include "extension.h"

using namespace Lya::Types;

namespace Lya::Extension {
	class ExtensionTestRunner {
	public:
		ExtensionTestRunner(shared_ptr<Session> session);
		void run_extension_tests();
	private:
		shared_ptr<Extension> extension;
		string current_test_file;
		string current_localization_file;
		shared_ptr<Session> session;
		string get_file_to_localization_json_string(const FileToLocalizations& file_to_localizations);
		void check_error_file(const string& test_name, const string& test_file, const vector<Diagnostic>& diagnostics);
		void check_localization_file(const string& test_name, const FileToLocalizations& file_to_localizations);
		void for_each_key_extraction_test(function<void(const string &)> callback);
		void for_each_compile_test(function<void(const string &)> callback);
		void start_extension_server();
	};
	void kill_all_processes(int signum);
} // Lya::Extension

#endif // EXTENSION_TEST_RUNNER_H
