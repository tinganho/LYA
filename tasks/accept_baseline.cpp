
#include "utils.cpp"
#include "configurations.h"

using namespace Lya::Utils;

int main() {
	remove_dir(PROJECT_DIR "src/program/tests/references");
    copy_folder(PROJECT_DIR "src/program/tests/currents", PROJECT_DIR "src/program/tests/references");
}
