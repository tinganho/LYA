
#include "utils.cpp"
#include "../src/configurations.h"
#include <string>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>
#include <json/json.h>

using namespace std;
using namespace Lya;
using namespace Lya::lib::utils;

const string start_wrap_header =
    "// This code is auto generate. Don't edit it!\n"
    "#ifndef LYA_DIAGNOSTICS_H\n"
    "#define LYA_DIAGNOSTICS_H\n"
    "\n"
    "#include \"types.h\"\n"
    "\n"
    "using namespace std;\n"
    "using namespace Lya::lib::types;\n"
    "\n"
    "namespace Lya::javascript_extension::diagnostics {\n"
    "\n"
    "class D {\n"
    "public:\n";

const string start_wrap_source =
    "// This code is auto generate. Don't edit it!\n"
    "\n"
    "#include \"types.h\"\n"
    "#include \"diagnostics.h\"\n"
    "\n"
    "using namespace std;\n"
    "using namespace Lya::lib::types;\n"
    "\n"
    "namespace Lya::javascript_extension::diagnostics {\n"
    "\n";

const string end_wrap_header =
    "};\n"
    "\n"
    "} // Lya::diagnostics \n"
    "\n"
    "#endif // DIAGNOSTICS_H";

const string end_wrap_source =
    "\n"
    "} // Lya::diagnostics \n";

vector<string> keys = {};

bool is_unique(string key) {
    for (auto const& k : keys) {
        if (k == key) {
            return false;
        }
    }
    return true;
}

string format_diagnostic_key(string key) {
    string k = boost::regex_replace(key, boost::regex("\\s+"), "_");
    k = boost::regex_replace(k, boost::regex("[\\.|\\'|:|,|.]"), "");
    k = boost::regex_replace(k, boost::regex("{(\\d+)}"), "$1");
    k = boost::regex_replace(k, boost::regex("_+"), "_");
    k = boost::regex_replace(k, boost::regex("^_+|_+$"), "");
    boost::match_results<std::string::const_iterator> results;
    if (boost::regex_search(k, boost::regex("[^a-zA-Z\\d_]"))) {
        throw invalid_argument("Your 'diagnostics.json' file contains non-alpha numeric characters: " + key);
    }

    return k;
}

string remove_comments(string json) {
    return boost::regex_replace(json, boost::regex("//.*?\n"), "\n");
}

int main() {
    string json = read_file(PROJECT_DIR "src/Extensions/javascript/diagnostics.json");
    Json::Value diagnostics;
    Json::Reader reader;
    string header_file = start_wrap_header;
    string source_file = start_wrap_source;
    reader.parse(remove_comments(json).c_str(), diagnostics);
    for (Json::ValueIterator it = diagnostics.begin(); it != diagnostics.end(); ++it) {
	    string unformatted_key = it.key().asString();
        string formatted_key = format_diagnostic_key(unformatted_key);
        if (!is_unique(formatted_key)) {
            throw invalid_argument("Duplicate formatted key: " + formatted_key + ".");
        }
        header_file += "    static DiagnosticTemplate " + formatted_key + ";\n";
        source_file += "    DiagnosticTemplate D::" + formatted_key + " = " + "DiagnosticTemplate { \"" + unformatted_key + "\" };\n";
        keys.push_back(formatted_key);
    }
    header_file += end_wrap_header;
    source_file += end_wrap_source;
    write_file(PROJECT_DIR "src/extensions/javascript/diagnostics.cpp", source_file);
    write_file(PROJECT_DIR "src/extensions/javascript/diagnostics.h", header_file);
    cout << "Successfully generated new diagnostics." << endl;
}
