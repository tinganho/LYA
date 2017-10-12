
#ifndef EXTENSION_TEST_RUNNER_H
#define EXTENSION_TEST_RUNNER_H

#include "Types.h"

using namespace Lya::Types;

namespace Lya::Extension {
	class ExtensionTestRunner {
	public:
		ExtensionTestRunner(shared_ptr<Session> session);
		void run_extension_tests();
	private:
		string current_localization_file_path;
		shared_ptr<Session> session;
		string get_file_to_localization_json_string(const FileToLocalizations& file_to_localizations);
		void check_error_file(const string& test_name, const vector<Diagnostic>& diagnostics);
		void check_localization_file(const string& test_name, const FileToLocalizations& file_to_localizations);
	};
	void kill_all_processes(int signum);
} // Lya::Extension

#endif // EXTENSION_TEST_RUNNER_H
