
#include <iostream>
#include "command_controller.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
	    return Lya::program::init(argc, argv);
    }
    catch (const exception& ex) {
        cerr << ex.what() << endl;
        return 1;
    }
}
