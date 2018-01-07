
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

	const std::map<PluralForm, std::string> plural_form_to_string = {
		{ PluralForm::Zero, "0" },
		{ PluralForm::One, "1" },
		{ PluralForm::Two, "2" },
		{ PluralForm::Few, "f" },
		{ PluralForm::Many, "m" },
		{ PluralForm::Other, "o" },
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
		TextWriter w;
		JavaScriptLanguage javascript_language;
		std::unique_ptr<MessageParser> message_parser;
		/// Bit flags of PluralCategory
		int supported_plural_categories;
		/// Bit flags of OrdinalCategory
		int supported_ordinal_categories;
		int current_unique_identifier;
        bool scan_dependencies;
        bool should_write_plural_form_resolver;
        bool should_write_integer_digits_value_transform_function;
        bool should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function;
        bool should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function;
        bool should_write_visible_fractional_digits_with_trailing_zero_value_transform_function;
        bool should_write_visible_fractional_digits_without_trailing_zero_value_transform_function;
		void write_integer_digits_value_transform_function();
		void write_number_of_fraction_digits_with_trailing_zero_value_transform_function();
		void write_number_of_fraction_digits_without_trailing_zero_value_transform_function();
		void write_visible_fractional_digits_with_trailing_zero_value_transform_function();
		void write_visible_fractional_digits_without_trailing_zero_value_transform_function();
		std::string generate_unique_identifier();
		void write_plural_form_resolver();
		void write_messages(const std::vector<std::unique_ptr<Message>>& messages);
		void compile_messages(const std::vector<LocalizationMessage>& messages);
        void write_dependent_functions();
		void write_localization_functions(const std::string id, const std::vector<Parameter> params, const Messages& messages);
	};
}

#endif // LYA_JAVASCRIPT_COMPILER_H