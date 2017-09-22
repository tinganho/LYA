
#include "ProjectTestRunner.h"
#include "Core.h"
#include "Utils.h"
#include "Configurations.h"
#include <fstream>
#include <exception>

using namespace std;
using namespace Lya::Utils;

namespace Lya::TestFramework {

void add_project_tests() {
    auto paths = find_files("src/Tests/Cases/Projects/*", PROJECT_DIR);

    domain("Project Tests");

    for (auto const &p : paths) {
        auto command = read_file(p + "/Command.cmd");
        string currents_dir = replace_string(p, "/Cases/", "/Currents/");
        recursively_create_dir(currents_dir);
        command = string(PROJECT_DIR) + "/bin/lya --rootDir " + currents_dir + " " + command;
        string result = execute_command(command);
        write_file(currents_dir + "/Output.txt", result);
        string test_name = p.substr(p.find_last_of("/") + 1);
        test(test_name, [result, p](Test* t) {
            string references_dir = replace_string(p, "/Cases/", "/References/");
            string reference = read_file(references_dir + "/Output.txt");
            if (result != reference) {
                throw runtime_error("Assertion Error!");
            }
        });
    }
}

} // Lya::TestFramework
