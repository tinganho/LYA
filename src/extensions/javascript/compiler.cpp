
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

	vector<Diagnostic> Compiler::compile(const std::vector<string> &localization_files)
	{
		std::vector<Diagnostic> diagnostics = read_localization_files(localization_files);
		return diagnostics;
	}

	void Compiler::compile_messages(Messages &messages)
	{

	}

	void Compiler::visit(TextMessage *)
	{

	}
}