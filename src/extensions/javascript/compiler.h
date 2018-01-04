
#ifndef LYA_JAVASCRIPT_COMPILER_H
#define LYA_JAVASCRIPT_COMPILER_H

#include "parsers/message/syntaxes.h"
#include "parsers/message/message_parser.h"
#include "parsers/ldml/syntaxes.h"
#include "javascript_language.h"
#include "text_writer.h"
#include "types.h"

using namespace Lya::core::parsers::ldml;
using namespace Lya::core::parsers::message;
using namespace Lya::lib::types;
using namespace Lya::lib;

namespace Lya::javascript_extension {
	class Compiler : public MessageNodeVisitor, public LdmlNodeVisitor {
	public:
		Compiler(
			JavaScriptLanguage _language,
			int supported_plural_categories,
			int supported_ordinal_categories);
		std::vector<Diagnostic> compile(const std::vector<string>& localization_files, const std::string& language);
		void visit(const TextMessage* text_message);
		void visit(const PluralMessage* plural_message);
		void visit(const InterpolationMessage* interpolation_message);

		void visit(const Expression*);
		void visit(const TokenNode*);
		void visit(const IntegerLiteral*);
		void visit(const FloatLiteral*);
		void visit(const ValueTransform*);
		void visit(const BinaryExpression*);
	private:
		TextWriter text_writer;
		JavaScriptLanguage javascript_language;
		std::unique_ptr<MessageParser> message_parser;
		/// Bit flags of PluralCategory
		int supported_plural_categories;
		/// Bit flags of OrdinalCategory
		int supported_ordinal_categories;
		void write_plural_rule_resolver();
		void compile_messages(const std::vector<LocalizationMessage>& messages);
		void write_localization_functions(const std::string id, const std::vector<Parameter> params, const Messages& messages);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H