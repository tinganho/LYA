
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

	Compiler::Compiler(): text_writer() { };

	std::tuple<std::vector<LocalizationMessage>, std::vector<Diagnostic>> Compiler::compile(const std::vector<string>& localization_files)
	{
		std::vector<Diagnostic> diagnostics;
		std::vector<LocalizationMessage> localizations;
		std::tie(localizations, diagnostics) = read_localization_files(localization_files);

		if (diagnostics.size() > 0)
		{
			return std::make_tuple(localizations, diagnostics);
		}
		compile_messages(localizations);
		return std::make_tuple(localizations, diagnostics);
	}

	void Compiler::compile_messages(const std::vector<LocalizationMessage>& localizations)
	{
		text_writer.write_line("export namespace L {");
		text_writer.indent();
		for (const LocalizationMessage& l : localizations)
		{
			write_function(l.id, l.params, l.message);
		}
		text_writer.unindent();
		text_writer.write_line(("};"));
	}

	void Compiler::write_function(const std::string id, const std::vector<Parameter> params, const std::string& body)
	{
		text_writer.write(id + "(");
		for (const Parameter& p : params)
		{
			text_writer.write(p.name);
		}
		text_writer.write(") {");
		text_writer.indent();
		text_writer.write(body);
		text_writer.unindent();
		text_writer.write("}");
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