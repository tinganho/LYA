
#include <codecvt>
#include <locale>
#include <string>
#include <Server/Server.cpp>
#include "JavaScriptScanner.cpp"

using namespace std;
using namespace Lya::Extension;
using namespace Lya::Types;

namespace JavaScriptExtension {

vector<Localization> extract_localizations(string file) {
};

int main() {
    ExtensionServer extension_server("localhost:8888", extract_localizations);
};

}
