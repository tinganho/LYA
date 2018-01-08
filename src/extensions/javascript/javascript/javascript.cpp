
#include <vector>
#include <codecvt>
#include "extension_server/extension_server.h"
#include "javascript/extractor.h"
#include "javascript/compiler.h"

using namespace Lya::services;
using namespace Lya::lib::types;
using namespace Lya::javascript_extension;

std::tuple<std::vector<LocalizationLocation>, std::vector<Diagnostic>> extract(const std::string& file, const std::vector<Glib::ustring>& function_names, uint64_t start_line) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::JavaScript);
    return extractor.extract(start_line);
}

std::vector<Diagnostic> compile(const std::vector<std::string>& localization_files) {
	Compiler compiler(JavaScriptLanguage::JavaScript, 0, 0);
	return compiler.compile(localization_files, "en-US");
}

int main() {
    ExtensionServer extension_server("localhost:8888", extract, compile);
    extension_server.start_server(false);
    return 0;
}
