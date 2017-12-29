
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "types.h"
#include "text_writer.h"
#include "parsers/message/syntaxes.h"

using namespace Lya::core::parsers::message;
using namespace Lya::lib::types;
using namespace Lya::lib;

namespace Lya::javascript_extension {
	class Compiler : public Visitor {
	public:
		Compiler();
		std::tuple<std::vector<LocalizationMessage>, std::vector<Diagnostic>> compile(const std::vector<string>& localization_files);
		void visit(const TextMessage* text_message);
		void visit(const PluralMessage* plural_message);
		void visit(const InterpolationMessage* interpolation_message);
	private:
		TextWriter text_writer;
		void compile_messages(const std::vector<LocalizationMessage>& messages);
		void write_function(const std::string id, const std::vector<Parameter> params, const std::string& body);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H