
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "types.h"
#include "parsers/message/syntaxes.h"

using namespace Lya::core::parsers::message;

namespace Lya::javascript_extension {
	class Compiler : public Visitor {
	public:
		Compiler();
		vector<lib::types::Diagnostic> compile(const vector<string> &localization_files);
	private:
		void compile_messages(Messages &messages);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H