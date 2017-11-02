
#include "project_test_runner.h"
#include "definitions.h"
#include "utils.h"
#include "configurations.h"
#include <fstream>
#include <exception>

using namespace std;
using namespace Lya::Utils;

namespace Lya::TestFramework {

void add_project_tests() {
    auto paths = find_files("src/tests/cases/projects/*", PROJECT_DIR);

    domain("Project tests");

    for (auto const &p : paths) {
        auto command = read_file(p + "/command.cmd");
        string currents_dir = replace_string(p, "/cases/", "/currents/");
        recursively_create_dir(currents_dir);
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

} // Lya::TestFramework
