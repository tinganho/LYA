
#include "Utils.cpp"
#include "Configurations.h"

using namespace Lya;

int main() {
    remove_all(PROJECT_DIR "src/Tests/Reference");
    copy_folder(PROJECT_DIR "src/Tests/Current", PROJECT_DIR "src/Tests/Reference");
}
