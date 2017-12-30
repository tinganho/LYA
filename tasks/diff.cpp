
#include <cstdlib>
#include <string>
#include "../src/configurations.h"

using namespace std;

int main() {
    string current_dir = PROJECT_DIR "src/tests/currents/";
    string reference_dir = PROJECT_DIR "src/tests/references/";
    string cmd = "$DIFF " + reference_dir + " " + current_dir;
    system(cmd.c_str());
}
