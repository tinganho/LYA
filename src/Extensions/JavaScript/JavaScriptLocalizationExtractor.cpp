
#include "JavaScriptLocalizationExtractor.h"

#include <tuple>
#include "Types.h"
#include "JavaScriptTokenScanner.h"

using namespace std;
using namespace Lya::Types;

namespace Lya::JavaScriptExtension {

JavaScriptLocalizationExtractor::JavaScriptLocalizationExtractor(
    const string _file,
    const vector<string> _function_names):
    scanner(_file),
    t(Token::None),
    function_names() {

    for (const auto &fn: _function_names) {
        function_names.push_back(scanner.to_u32_string(fn));
    }
}

tuple<vector<Localization>> JavaScriptLocalizationExtractor::extract() {
    vector<Localization> localizations;
    while (true) {
        Token t = next_token();
        switch (t) {
            case Token::Identifier: {
                u32string accessor;
                bool success = try_scan_localization_function_accessor(get_value(), accessor);
                if (success && scan_expected(Token::Dot)) {
                    if (scan_expected(Token::Identifier)) {
	                    u32string localization_id = get_value();
                        if (scan_expected(Token::OpenParen)) {
                            Localization localization(
                                scanner.to_utf8_string(localization_id),
                                scanner.line,
                                scanner.column
                            );
                            scan_parameter_list();
                        }
                    }
                }
                break;
            }

            case Token::EndOfFile:
                goto End;

            default:
                continue;
        }

        End:
        break;
    }

    return make_tuple(localizations);
}

void JavaScriptLocalizationExtractor::scan_parameter_list() {

}

bool JavaScriptLocalizationExtractor::is_localization_function_name(const u32string &function_name) {
    for (const u32string& fn : function_names) {
        if (function_name == fn) {
            return true;
        }
    }
    return false;
}

bool JavaScriptLocalizationExtractor::try_scan_localization_function_accessor(const u32string &accessor,
                                             u32string &localization_function_name_placeholder) {
    scanner.save();
    if (is_localization_function_name(accessor) && next_token() == Token::Dot) {
        localization_function_name_placeholder = accessor;
        scanner.revert();
        return true;
    }
    if (next_token() == Token::Identifier) {
        return try_scan_localization_function_accessor(accessor + U"." + get_value(),
                                                       localization_function_name_placeholder);
    }
    scanner.revert();
    return false;
}

bool JavaScriptLocalizationExtractor::scan_expected(const Token &_t) {
    if (next_token() != _t) {
        return true;
    }
    return false;
}

Token JavaScriptLocalizationExtractor::next_token() {
    return t = scanner.next_token();
}

u32string JavaScriptLocalizationExtractor::get_value() {
    return scanner.get_value();
}

} // Lya::JavaScriptExtension

