
#include "utils.cpp"
#include "configurations.h"

using namespace Lya::Utils;

int main() {
	remove_dir(PROJECT_DIR "src/tests/Reference");
    copy_folder(PROJECT_DIR "src/tests/Current", PROJECT_DIR "src/tests/Reference");
}
