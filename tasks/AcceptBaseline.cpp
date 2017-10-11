
#include "Utils.cpp"
#include "Configurations.h"

using namespace Lya::Utils;

int main() {
	remove_dir(PROJECT_DIR "src/Tests/Reference");
    copy_folder(PROJECT_DIR "src/Tests/Current", PROJECT_DIR "src/Tests/Reference");
}
