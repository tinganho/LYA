
#include "JavaScriptLocalizationExtractor.h"

#include <tuple>
#include "Types.h"
#include "JavaScriptTokenScanner.h"
#include "Diagnostics.h"
#include "Utils.h"

using namespace std;


namespace Lya::JavaScriptExtension {

using namespace Types;
using namespace Utils;
using namespace Diagnostics;

const vector<u32string> types = { U"string", U"number", U"Date" };

JavaScriptLocalizationExtractor::JavaScriptLocalizationExtractor(
    const string _file,
    const vector<string> _function_names,
	JavaScriptLanguage _language):
    scanner(_file),
    diagnostics(),
    t(Token::None),
    language(_language),
    function_names() {

    for (const auto &fn: _function_names) {
        function_names.push_back(to_u32_string(fn));
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
                        vector<Param> params = scan_parameter_list();
                        Localization l {
                            to_utf8_string(localization_id),
                            params,
                            scanner.line,
                            scanner.column,
                        };
                        localizations.push_back(l);
                    }
                }
                break;
            }

            case Token::EndOfFile:
                goto return_statement;

            default:;
        }

    }

	return_statement:

    return make_tuple(localizations);
}

vector<Param> JavaScriptLocalizationExtractor::scan_parameter_list() {
	vector<Param> params;
	string captured_type;
	if (scan_expected(Token::OpenParen)) {
		while (true) {
			Token t = next_token();
			switch (t) {
				case Token::Identifier:
					if (language == JavaScriptLanguage::TypeScript) {
						add_diagnostic(get_location(), D::You_must_provide_a_type_comment_for_the_argument_0, get_utf8_value());
						continue;
					}
					Param p { to_utf8_string(get_value()), captured_type, false };
					params.push_back(p);
					break;
				case Token::SlashAsterix: {
					captured_type = scan_type();
					scan_expected(Token::AsterixSlash);
					scan_expected(Token::Identifier);
					Param p { to_utf8_string(get_value()), captured_type, false };
					params.push_back(p);
					break;
				}
				case Token::EndOfFile:
					add_diagnostic(scanner.get_location(), D::Unexpected_end_of_file);
				case Token::CloseParen:
					goto return_statement;
				default:;
			}
		}
	}

	return_statement:
	return params;
}

bool JavaScriptLocalizationExtractor::is_localization_function_name(const u32string &function_name) {
    for (const u32string& fn : function_names) {
        if (function_name == fn) {
            return true;
        }
    }
    return false;
}

bool JavaScriptLocalizationExtractor::try_scan_localization_function_accessor(
    const u32string &accessor,
    u32string &localization_function_name_placeholder) {

    scanner.save();
    if (is_localization_function_name(accessor) && next_token() == Token::Dot) {
        localization_function_name_placeholder = accessor;
        scanner.revert();
        return true;
    }
    if (next_token() == Token::Identifier) {
        return try_scan_localization_function_accessor(
            accessor + U"." + get_value(),
            localization_function_name_placeholder);
    }
    scanner.revert();
    return false;
}

bool JavaScriptLocalizationExtractor::scan_expected(const Token& expected_token) {
	Token current_token = next_token();
    if (current_token == expected_token) {
        return true;
    }
	add_diagnostic(scanner.get_location(),
	               D::Expected_0_but_got_1,
	               token_enum_to_string.find(current_token)->second,
	               token_enum_to_string.find(expected_token)->second);
    return false;
}

string JavaScriptLocalizationExtractor::scan_type() {
	return scan_type(false);
}

string JavaScriptLocalizationExtractor::scan_type(bool expect_type) {
	scan_expected(Token::Identifier);
	u32string type = get_value();
	if (is_of_supported_type(type)) {
		if (peek_next_token() == Token::OpenBracket) {
			next_token();
			scan_expected(Token::CloseBracket);
			type += U"[]";
		}
		return to_utf8_string(type);
	}
	else if (!expect_type && peek_next_token() == Token::Colon) {
		next_token();
		return scan_type(/*expect_type*/ true);
	}
	add_diagnostic(scanner.get_location(), D::Unknown_type_definition_0, to_utf8_string(type));
}

bool JavaScriptLocalizationExtractor::is_of_supported_type(const u32string& type) {
	for (const auto& t : types) {
		if (t == type) {
			return true;
		}
	}
	return false;
}

Token JavaScriptLocalizationExtractor::next_token() {
    return t = scanner.next_token();
}

Token JavaScriptLocalizationExtractor::peek_next_token() {
	scanner.save();
	const Token t = next_token();
	scanner.revert();
	return t;
}

u32string JavaScriptLocalizationExtractor::get_value() const {
    return scanner.get_value();
}

string JavaScriptLocalizationExtractor::get_utf8_value() {
	return to_utf8_string(get_value());
}

Location JavaScriptLocalizationExtractor::get_location() const {
	return scanner.get_location();
};

string JavaScriptLocalizationExtractor::to_utf8_string(const u32string& str) {
	return scanner.to_utf8_string(str);
}

u32string JavaScriptLocalizationExtractor::to_u32_string(const string& str) {
	return scanner.to_u32_string(str);
}

void JavaScriptLocalizationExtractor::add_diagnostic(Location location, DiagnosticTemplate _template) {
	diagnostics.push_back(create_diagnostic(location, _template));
}

void JavaScriptLocalizationExtractor::add_diagnostic(Location location, DiagnosticTemplate _template, string arg1) {
	diagnostics.push_back(create_diagnostic(location, _template, arg1));
}

void JavaScriptLocalizationExtractor::add_diagnostic(Location location, DiagnosticTemplate _template, string arg1, string arg2) {
	diagnostics.push_back(create_diagnostic(location, _template, arg1, arg2));
}

} // Lya::JavaScriptExtension
