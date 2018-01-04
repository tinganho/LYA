
#include "compiler.h"
#include "utils.h"
#include "diagnostics.h"
#include "parsers/message/message_parser.h"
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
			switch (pc) {
				case PluralCategory::One:
					std::unique_ptr<Expression>& expression = message_parser->plural_rules->at(PluralCategory::One);
					expression->accept(this);
					text_writer.write("");

			}
		}
	}

	/// LDML Nodes

	void Compiler::visit(const Expression* expression)
	{

	}

	void Compiler::visit(const TokenNode* token_node)
	{

	}

	void Compiler::visit(const IntegerLiteral* integer_literal)
	{

	}

	void Compiler::visit(const FloatLiteral* float_literal)
	{

	}

	void Compiler::visit(const ValueTransform* value_transform)
	{

	}

	void Compiler::visit(const BinaryExpression* binary_expression)
	{

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