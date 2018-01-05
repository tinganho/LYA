
#include "compiler.h"
#include "diagnostics.h"
#include "parsers/ldml/ldml_parser.h"
#include "lib/external_data.h"
#include <libxml++/libxml++.h>

using namespace Lya::lib;
using namespace Lya::lib::utils;
using namespace Lya::lib::types;
using namespace Lya::core::parsers::ldml;
using namespace Lya::core::parsers::message;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {

	Compiler::Compiler(JavaScriptLanguage _javascript_language, int _supported_plural_categories, int _supported_ordinal_categories):
		javascript_language(_javascript_language),
		supported_plural_categories(_supported_plural_categories),
		supported_ordinal_categories(_supported_ordinal_categories),
		should_write_integer_digits_value_transform_function(false),
		text_writer()
	{ }

	std::vector<Diagnostic> Compiler::compile(const std::vector<string>& localization_files, const std::string& language)
	{
		std::vector<Diagnostic> diagnostics;
		std::vector<LocalizationMessage> localizations;
		std::tie(localizations, diagnostics) = read_localization_files(localization_files);
		if (diagnostics.size() > 0) {
			return diagnostics;
		}
		message_parser = std::make_unique<MessageParser>(language);
		compile_messages(localizations);
		return diagnostics;
	}

	void Compiler::compile_messages(const std::vector<LocalizationMessage>& localizations)
	{
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			text_writer.write_line("export namespace L {");
		}
		else {
			text_writer.write_line("export const L {");
		}
		text_writer.indent();
		for (const LocalizationMessage& l : localizations) {
			Messages messages = message_parser->parse(l.message);
			write_localization_functions(l.id, l.params, messages);
		}
		text_writer.unindent();
		text_writer.write_line(("};"));
	}

	void Compiler::write_localization_functions(
		const std::string id,
		const std::vector<Parameter> params,
        const Messages &messages)
	{
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			text_writer.write("export function " + id + "(");
		}
		else {
			text_writer.write(id + ": function(");
		}
		for (const Parameter& p : params) {
			if (javascript_language == JavaScriptLanguage::TypeScript) {
				text_writer.write(p.name);
			}
			else {
				text_writer.write(p.name + ": " + *p.type);
			}
		}
		text_writer.write(")");
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			text_writer.write(": void");
		}
		text_writer.write_line(" {");
		text_writer.indent();
		for (const shared_ptr<Message> message : messages) {
			message->accept(this);
		}
		text_writer.unindent();
		text_writer.write_line("},");
	}

	void Compiler::write_plural_rule_resolver()
	{
		message_parser->read_plural_info();
		for (const PluralCategory& pc : *message_parser->supported_plural_categories) {
			std::unique_ptr<Expression>& ldml_expression = message_parser->plural_rules->at(PluralCategory::One);
			ldml_expression->accept(this);
		}
	}

	std::string Compiler::to_js_string(LdmlToken ldml_token)
	{
		return ldml_token_enum_to_javascript_string.at(ldml_token);
	}

	void Compiler::write_integer_digits_value_transform_function()
	{
		text_writer.write_line("function n(v) {");
		text_writer.indent();
		text_writer.write_line("return Math.floor(v);");
		text_writer.unindent();
		text_writer.write_line("}");
	}

	void Compiler::write_number_of_fraction_digits_with_trailing_zero_value_transform_function()
	{
		text_writer.write_line("function _v(v) {");
		text_writer.indent();
		text_writer.write_line("var s = v.toString().split('.');");
		text_writer.write_line("if (s.length > 0) {");
		text_writer.indent();
		text_writer.write_line("return s[1].length;");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return 0;");
		text_writer.unindent();
		text_writer.write_line("}");
	}

	void Compiler::write_number_of_fraction_digits_without_trailing_zero_value_transform_function()
	{
		text_writer.write_line("function w(v) {");
		text_writer.indent();
		text_writer.write_line("var s = v.toString().split('.');");
		text_writer.write_line("if (s.length > 0) {");
		text_writer.indent();
		text_writer.write_line("var l = s[1].length;");
		text_writer.write_line("for (var i = l - 1; i >= 0; i--) {");
		text_writer.indent();
		text_writer.write_line("if (s[1][i] === '0') {");
		text_writer.indent();
		text_writer.write_line("l--;");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return l");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return 0;");
		text_writer.unindent();
		text_writer.write_line("}");
	}

	void Compiler::write_visible_fractional_digits_with_trailing_zero_value_transform_function()
	{
		text_writer.write_line("function f(v) {");
		text_writer.indent();
		text_writer.write_line("var s = v.toString().split('.');");
		text_writer.write_line("if (s.length > 0) {");
		text_writer.indent();
		text_writer.write_line("return parseInt(s[1], 10);");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return 0;");
		text_writer.unindent();
		text_writer.write_line("}");
	}

	void Compiler::write_visible_fractional_digits_without_trailing_zero_value_transform_function()
	{
		text_writer.write_line("function w(v) {");
		text_writer.indent();
		text_writer.write_line("var s = v.toString().split('.');");
		text_writer.write_line("if (s.length > 0) {");
		text_writer.indent();
		text_writer.write_line("for (var i = s[1].length - 1; i >= 0; i--) {");
		text_writer.indent();
		text_writer.write_line("if (s[1][i] === '0') {");
		text_writer.indent();
		text_writer.write_line("delete s[1][i];");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return parseInt(s[1], 10);");
		text_writer.unindent();
		text_writer.write_line("}");
		text_writer.write_line("return 0;");
		text_writer.unindent();
		text_writer.write_line("}");
	}

	/// LDML Nodes

	void Compiler::visit(const Expression* expression)
	{
		throw logic_error("should not reach here.");
	}

	void Compiler::visit(const TokenNode* token_node)
	{
		text_writer.write(" " +  to_js_string(token_node->token) + " ");
	}

	void Compiler::visit(const IntegerLiteral* integer_literal)
	{
		text_writer.write(std::to_string(integer_literal->value));
	}

	void Compiler::visit(const FloatLiteral* float_literal)
	{
		text_writer.write(std::to_string(float_literal->value));
	}

	void Compiler::visit(const ValueTransform* value_transform)
	{
		switch (value_transform->type) {
			case ValueTransformType::AbsoluteValue:
				text_writer.write_line("v");
				break;
			case ValueTransformType::IntegerDigitsValue:
				text_writer.write_line("i(v)");
				should_write_integer_digits_value_transform_function = true;
				break;
			case ValueTransformType::NumberOfVisibleFractionDigits_WithTrailingZeros:
				text_writer.write_line("_v(v)");
				should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function = true;
				break;
			case ValueTransformType::NumberOfVisibleFractionDigits_WithoutTrailingZeros:
				text_writer.write_line("w(v)");
				should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function = true;
				break;
			case ValueTransformType::VisibleFractionalDigits_WithTrailingZeros:
				text_writer.write_line("f(v)");
				break;
			case ValueTransformType::VisibleFractionalDigits_WithoutTrailingZeros:
				text_writer.write_line("t(v)");
				break;
		}
	}

	void Compiler::visit(const BinaryExpression* binary_expression)
	{
		binary_expression->left_operand->accept(this);
		binary_expression->_operator->accept(this);
		binary_expression->right_operand->accept(this);
	}

	/// Message Nodes

	void Compiler::visit(const TextMessage* text_message)
	{
		text_writer.write_line(" + \"" + text_message->text + "\"");
	}

	void Compiler::visit(const InterpolationMessage* interpolation_message)
	{
		text_writer.write(" + \"" + interpolation_message->variable);
	}

	void Compiler::visit(const PluralMessage* plural_message)
	{
		text_writer.newline();

//		plural_message->plural_category_messages
//		text_writer.write_line();
	}
}