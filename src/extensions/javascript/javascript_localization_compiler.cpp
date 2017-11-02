
#include "javascript_localization_compiler.h"
#include "utils.h"

namespace Lya::JavaScriptExtension {
	using namespace Utils;

	Compiler::Compiler() {
		string path = get_exec_path();
		cout << path << endl;

	}
}