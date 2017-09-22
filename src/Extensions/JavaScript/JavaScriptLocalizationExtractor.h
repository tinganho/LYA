
#ifndef JAVASCRIPT_LOCALIZATION_EXTRACTOR_H
#define JAVASCRIPT_LOCALIZATION_EXTRACTOR_H

#include <tuple>
#include <Types.h>
#include "JavaScriptTokenScanner.h"

using namespace std;
using Lya::Types::Localization;

namespace Lya::JavaScriptExtension {

class JavaScriptLocalizationExtractor {
public:
    JavaScriptLocalizationExtractor(const string _file, const vector<string> _function_names);
    tuple<vector<Localization>> extract();

private:
    JavaScriptTokenScanner scanner;
    Token t;
    vector<u32string> function_names;

    void scan_parameter_list();
    bool is_localization_function_name(const u32string &function_name);
    bool try_scan_localization_function_accessor(const u32string &accessor, u32string &localization_function_name_placeholder);
    bool scan_expected(const Token &_t);
    Token next_token();
    u32string get_value();
};

} // Lya::JavaScriptExtension

#endif // JAVASCRIPT_LOCALIZATION_EXTRACTOR_H
