

#include "utils.h"
#include "project_test_runner.h"
#include "configurations.h"
#include "definitions.h"

using namespace std;
using namespace Lya::Utils;
using namespace Lya::TestFramework;

int main() {
    try {
	    remove_dir(PROJECT_DIR "src/program/tests/currents");
        add_project_tests();
        run_tests();
        return print_result();
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
