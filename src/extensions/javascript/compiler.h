
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "types.h"

using namespace Lya::lib::types;

namespace Lya::javascript_extension {
	class Compiler {
	public:
		Compiler();
		vector<lib::types::Diagnostic> compile(const vector<string>& localization_files);
	private:
		string dtd_file;

		string get_dtd_file();
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H