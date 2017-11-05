
#include "javascript_localization_compiler.h"
#include "utils.h"

using namespace Lya::utils;

namespace Lya::javascript_extension {

	Compiler::Compiler() {
		string path = get_exec_path();
		cout << path << endl;

	}
}