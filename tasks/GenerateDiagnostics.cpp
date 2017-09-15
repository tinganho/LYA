
#include "Utils.cpp"
#include "Configurations.h"
#include <string>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>
#include <json/json.h>

using namespace std;
using namespace Lya;
using namespace Lya::Utils;

string output =
    "// This code is auto generate. Don't edit it!\n"
    "#ifndef DIAGNOSTICS_H\n"
    "#define DIAGNOSTICS_H\n"
    "\n"
    "#include \"Types.cpp\"\n"
    "\n"
    "using namespace std;\n"
    "using namespace Lya::Types;\n"
    "\n"
    "namespace Lya {\n"
    "namespace Diagnostics {\n"
    "\n"
    "namespace D {\n";

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
    k = boost::regex_replace(k, boost::regex("[\\.|\\'|:]"), "");
    k = boost::regex_replace(k, boost::regex("{\\d+}"), "");
    k = boost::regex_replace(k, boost::regex("_+"), "_");
    k = boost::regex_replace(k, boost::regex("^_+|_+$"), "");
    boost::match_results<std::string::const_iterator> results;
    if (boost::regex_search(k, boost::regex("[^a-zA-Z\\d_:]"))) {
        throw invalid_argument("Your 'Diagnostics.json' file contains non-alpha numeric characters: " + key);
    }

    return k;
}

string remove_comments(string json) {
    return boost::regex_replace(json, boost::regex("//.*?\n"), "\n");
}

int main() {
    string json = read_file(PROJECT_DIR "src/Program/Diagnostics.json");
    Json::Value diagnostics;
    Json::Reader reader;
    reader.parse(remove_comments(json).c_str(), diagnostics);
    for (Json::ValueIterator it = diagnostics.begin(); it != diagnostics.end(); ++it) {
        string key = format_diagnostic_key(it.key().asString());
        output += "    DiagnosticTemplate " + key + "(\"" + key + "\");\n";
        if (!is_unique(key)) {
            throw invalid_argument("Duplicate formatted key: " + key + ".");
        }
        keys.push_back(key);
    }

    output += "} // D \n";
    output += "\n";
    output += "} // Diagnostics \n";
    output += "} // Lya \n";
    output += "#endif";

    write_file(PROJECT_DIR "src/Program/Diagnostics.cpp", output);
    cout << "Successfully generated new diagnostics." << endl;
}
