
#include <vector>
#include <codecvt>
#include "../../../services/extension_server/extension_server.h"
#include "javascript/extractor.h"

using namespace std;
using namespace lya::services;
using namespace lya::types;
using namespace lya::javascript_extension;

tuple<vector<Localization>, vector<Diagnostic>> extract(const string &file, const vector<string> &function_names,
                                                        uint64_t start_line) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::TypeScript);
    return extractor.extract(start_line);
}

int main() {
    ExtensionServer extension_server("localhost:8888", extract);
    extension_server.start_server(false);
    return 0;
}
