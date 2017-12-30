
#include "compiler.h"
#include "utils.h"
#include "diagnostics.h"
#include "parsers/message/message_parser.h"
#include "localization_file_reader.h"
#include <libxml++/libxml++.h>

using namespace Lya::lib::utils;
using namespace Lya::lib::types;
using namespace Lya::core::parsers::message;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {

	Compiler::Compiler(JavaScriptLanguage _javascript_language, int _supported_plural_categories, int _supported_ordinal_categories):
		javascript_language(_javascript_language),
		supported_plural_categories(_supported_plural_categories),
		supported_ordinal_categories(_supported_ordinal_categories),
		text_writer()
	{ };

	std::vector<Diagnostic> Compiler::compile(const std::vector<string>& localization_files)
	{
		std::vector<Diagnostic> diagnostics;
		std::vector<LocalizationMessage> localizations;
		std::tie(localizations, diagnostics) = read_localization_files(localization_files);

		if (diagnostics.size() > 0) {
			return diagnostics;
		}
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
			MessageParser message_parser;
			Messages messages = message_parser.parse(l.message, "en-US");
			write_localization_functions(l.id, l.params, messages);
		}
		text_writer.unindent();
		text_writer.write_line(("};"));
//		std::cout << text_writer.text << std::endl;
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