
#include "project_test_runner.h"
#include "test_definitions.h"
#include "utils.h"
#include "configurations.h"
#include <fstream>
#include <exception>

using namespace std;
using namespace Lya::lib::test_definitions;
using namespace Lya::lib::utils;

void add_project_tests() {
    auto paths = find_files("src/tests/cases/projects/*", PROJECT_DIR);

    domain("Project tests");

    for (auto const &p : paths) {
        auto command = read_file(p + "/command.cmd");
        string currents_dir = replace_string(p, "/cases/", "/currents/");
	    recursively_create_folder(currents_dir);
        command = string(PROJECT_DIR) + "/bin/lya --root-dir " + currents_dir + " " + command;
        string result = execute_command(command);
        write_file(currents_dir + "/output.txt", result);
        string test_name = p.substr(p.find_last_of("/") + 1);
        test(test_name, [result, p](Test* t) {
            string references_dir = replace_string(p, "/cases/", "/references/");
            string reference = read_file(references_dir + "/output.txt");
            if (result != reference) {
                throw runtime_error("Assertion Error!");
            }
        });
    }
}

int main() {
	try {
		remove_folder(PROJECT_DIR "src/program/tests/currents");
		add_project_tests();
		run_tests();
		return print_result();
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
}