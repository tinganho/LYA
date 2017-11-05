
#ifndef LYA_JAVASCRIPT_LOCALIZATION_EXTRACTOR_H
#define LYA_JAVASCRIPT_LOCALIZATION_EXTRACTOR_H

#include "types.h"

using namespace Lya::types;

namespace Lya::javascript_extension {
	class Compiler {
	public:
		Compiler();
		vector<Diagnostic> compile(const vector<string>& localization_file_paths);
	};
}

#endif