
#include <vector>
#include <codecvt>
#include <Server/Server.h>
#include "JavaScriptLocalizationExtractor.h"

using namespace std;
using namespace Lya::Extension;
using namespace Lya::Types;
using namespace Lya::JavaScriptExtension;

tuple<vector<Localization>, vector<Diagnostic>> extract_localizations(const string& file, const vector<string>& function_names, uint64_t start_line) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::JavaScript);
    return extractor.extract(start_line);
}

int main() {
    ExtensionServer extension_server("localhost:8888", extract_localizations);
    extension_server.start_server(false);
    return 0;
}
