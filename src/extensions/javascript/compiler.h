
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "parsers/message/syntaxes.h"
#include "javascript_language.h"
#include "text_writer.h"
#include "types.h"

using namespace Lya::core::parsers::message;
using namespace Lya::lib::types;
using namespace Lya::lib;

namespace Lya::javascript_extension {
	class Compiler : public Visitor {
	public:
		Compiler(
			JavaScriptLanguage _language,
			int supported_plural_categories,
			int supported_ordinal_categories);
		std::vector<Diagnostic> compile(const std::vector<string>& localization_files);
		void visit(const TextMessage* text_message);
		void visit(const PluralMessage* plural_message);
		void visit(const InterpolationMessage* interpolation_message);
	private:
		TextWriter text_writer;
		JavaScriptLanguage javascript_language;
		int supported_plural_categories;
		int supported_ordinal_categories;
		void compile_messages(const std::vector<LocalizationMessage>& messages);
		void write_localization_functions(const std::string id, const std::vector<Parameter> params, const Messages& messages);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H