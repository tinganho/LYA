
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

	const std::map<LdmlToken, std::string> ldml_token_enum_to_javascript_string = {
		{ LdmlToken::LogicalAnd, "&&" },
		{ LdmlToken::LogicalOr, "||" },
		{ LdmlToken::Equality, "=" },
		{ LdmlToken::Remainder, "%" },
	};

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
		bool should_write_integer_digits_value_transform_function;
		bool should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function;
		bool should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function;
		bool should_write_visible_fractional_digits_with_trailing_zero_value_transform_function;
		bool should_write_visible_fractional_digits_without_trailing_zero_value_transform_function;
		/// Bit flags of PluralCategory
		int supported_plural_categories;
		/// Bit flags of OrdinalCategory
		int supported_ordinal_categories;
		std::string to_js_string(LdmlToken ldml_token);
		void write_integer_digits_value_transform_function();
		void write_number_of_fraction_digits_with_trailing_zero_value_transform_function();
		void write_number_of_fraction_digits_without_trailing_zero_value_transform_function();
		void write_visible_fractional_digits_with_trailing_zero_value_transform_function();
		void write_visible_fractional_digits_without_trailing_zero_value_transform_function();
		void write_plural_rule_resolver();
		void compile_messages(const std::vector<LocalizationMessage>& messages);
		void write_localization_functions(const std::string id, const std::vector<Parameter> params, const Messages& messages);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H