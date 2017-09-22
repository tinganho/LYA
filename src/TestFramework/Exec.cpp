

#include "Utils.h"
#include "ProjectTestRunner.h"
#include "Configurations.h"
#include "Core.h"

using namespace std;
using namespace Lya::Utils;
using namespace Lya::TestFramework;

int main() {
    try {
        remove_all(PROJECT_DIR "src/Tests/Currents");
        add_project_tests();
        run_tests();
        return print_result();
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
