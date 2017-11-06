
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "types.h"

using namespace Lya::types;

namespace Lya::javascript_extension {
	class Compiler {
	public:
		Compiler();
		vector<Diagnostic> compile(const vector<string>& localization_files);
	private:
		string dtd_file;

		string get_dtd_file();
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H