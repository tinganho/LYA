
#ifndef EXTENSION_TEST_RUNNER_H
#define EXTENSION_TEST_RUNNER_H

#include "types.h"
#include "extension.h"

using namespace Lya::types;

namespace Lya::extension {
	class ExtensionTestRunner {
	public:
		ExtensionTestRunner(shared_ptr<Session> session);
		void run_extension_tests();
	private:
		shared_ptr<Extension> extension;
		shared_ptr<Session> session;
		string get_file_to_localization_json_string(const FileToLocalizations& file_to_localizations);
		void check_error_file(const string& test_name, const string& canonical_test_file_path, const vector<Diagnostic>& diagnostics);
		void check_localization_file(const string& localization_file, const string& test_name, const FileToLocalizations& file_to_localizations);
		void visit_extraction_test(function<void(const string &)> callback);
		void visit_compile_test(function<void(const string &)> callback);
		void start_extension_server();
		string get_canonical_test_file_path(const string &test_file) const;
		bool should_skip_test(const string& current_canonical_file_path);
		void define_tests();
		void define_extraction_tests();
		void define_compile_tests();
	};
	void kill_all_processes(int signum);
} // Lya::Extension

#endif // EXTENSION_TEST_RUNNER_H
