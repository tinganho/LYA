
#include <vector>
#include <codecvt>
#include <Server/Server.h>
#include "JavaScriptLocalizationExtractor.h"

using namespace std;
using namespace Lya::Extension;
using namespace Lya::Types;
using namespace Lya::JavaScriptExtension;

vector<Localization> extract_localizations(const string& file, const vector<string>& function_names) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::JavaScript);
    auto result = extractor.extract();
    return get<vector<Localization>>(result);
}

int main() {
    ExtensionServer extension_server("localhost:8888", extract_localizations);
    extension_server.start_server(false);
    return 0;
}
