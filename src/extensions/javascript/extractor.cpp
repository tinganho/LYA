
#include "extractor.h"
#include <tuple>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/saxparser.h>
#include "types.h"
#include "token_scanner.h"
#include "diagnostics.h"
#include "utils.h"

using namespace std;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {

	const vector<u32string> types = { U"string", U"number", U"Date" };

	JavaScriptLocalizationExtractor::JavaScriptLocalizationExtractor(
	    const string& _file,
	    const vector<string> _function_names,
		JavaScriptLanguage _language):
	    scanner(from_utf8_to_u32(read_file(_file))),
	    t(Token::None),
	    language(_language),
	    function_names() {

	    for (const auto &fn: _function_names) {
	        function_names.push_back(to_u32_string(fn));
	    }
	}

	tuple<vector<Localization>, vector<::types::Diagnostic>> JavaScriptLocalizationExtractor::extract(uint64_t start_line) {
	    vector<Localization> localizations;

	    while (true) {
	        Token t = next_token();
		    if (t == Token::EndOfFile) {
			    break;
		    }
	        switch (t) {
	            case Token::Identifier: {
	                u32string accessor;
		            SpanLocation start_location = scanner.get_token_location();
	                bool success = try_scan_localization_function_accessor(get_value(), accessor);
	                if (success && scan_expected(Token::Dot)) {
	                    if (scan_expected(Token::Identifier)) {
		                    u32string localization_id = get_value();
	                        auto params = scan_parameter_list();
		                    if (!has_diagnostics) {
			                    bool ok = get<1>(params);
			                    if (!ok) {
				                    continue;
			                    }
			                    Localization l {
				                    to_utf8_string(localization_id),
				                    get<vector<Param>>(params),
				                    SpanLocation {
					                    start_location.line,
					                    start_location.column,
					                    scanner.position - start_location.position
				                    }
			                    };
			                    localizations.push_back(l);
		                    }
	                    }
	                }
	                break;
	            }

		        case Token::SlashAsterix:
			        scan_multiline_comment();
					break;

	            case Token::EndOfFile:
	                goto return_statement;

	            default:;
	        }

	    }

		return_statement:
		if (has_diagnostics) {
			return make_tuple(vector<Localization> {}, diagnostics);
		}
	    return make_tuple(localizations, diagnostics);
	}

	void JavaScriptLocalizationExtractor::scan_multiline_comment() {
		while (true) {
			Token t = next_token();
			switch (t) {
				case Token::AsterixSlash:
				case Token::EndOfFile:
					goto outer;
				default:;
			}
		}
		outer:;
	}

	tuple<vector<Param>, bool> JavaScriptLocalizationExtractor::scan_parameter_list() {
		vector<Param> params;
		bool ok = true;
		string captured_type;
		if (scan_expected(Token::OpenParen)) {
			while (true) {
				Token t = next_token(true, true);
				switch (t) {
					case Token::Identifier: {
						if (language == JavaScriptLanguage::TypeScript) {
							add_diagnostic(D::You_must_provide_a_type_comment_for_the_argument_0, get_utf8_value());
							ok = false;
							break;
						}
						string value = get_utf8_value();
						if (value == "null" || value == "undefined" || value == "true" || value == "false") {
							add_diagnostic(D::Argument_must_be_a_variable_or_defined_by_a_preceding_comment);
							ok = false;
							break;
						}
						Token peek = peek_next_token(true, true);
						if (peek != Token::Comma && peek != Token::CloseParen) {
							scan_invalid_argument();
							ok = false;
							break;
						}
						Param p { value, false };
						params.push_back(p);
						break;
					}
					case Token::SlashAsterix: {
						bool scanned_ok;
						IdentifierType identifier_type;
						tie(identifier_type, scanned_ok) = scan_type_and_or_identifier();
						if (!scanned_ok) {
							ok = false;
							break;
						}
						if (!scan_expected(Token::AsterixSlash)) {
							ok = false;
							break;
						}
						string identifier;
						if (identifier_type.identifier != nullptr) {
							if (!scan_expected(Token::Identifier)) {
								ok = false;
								break;
							}
							identifier = get_utf8_value();
						}
						else {
							identifier = *identifier_type.identifier;
						}

						Param p { identifier, false, *identifier_type.type };
						params.push_back(p);
						break;
					}
					case Token::String:
						scan_invalid_argument();
						ok = false;
						break;

					case Token::EndOfFile:
						add_diagnostic(D::Unexpected_end_of_file);
						goto return_statement;

					case Token::InvalidArgument:
						ok = false;
						add_diagnostic(D::Argument_must_be_a_variable_or_defined_by_a_preceding_comment);
						break;

					case Token::CloseParen:
						goto return_statement;

					case Token::Comma:
						break;

					default:
						ok = false;
						add_diagnostic(D::Argument_must_be_a_variable_or_defined_by_a_preceding_comment);
				}
			}
		}

		return_statement:
		return make_tuple(params, ok);
	}

	void JavaScriptLocalizationExtractor::scan_invalid_argument() {
		int start_position = scanner.start_position;
		SpanLocation location = scanner.get_token_location();
		Token t = next_token();
		if (t == Token::OpenParen) {
			int open_parens = 0;
			while (true) {
				if (t == Token::EndOfFile) {
					break;
				}
				switch (t) {
					case Token::OpenParen:
						open_parens++;
						break;
					case Token::CloseParen:
						if (--open_parens == 0) {
							goto outer;
						}
						break;
					default:;
				}
				t = next_token();
			}
		}
		else {
			while (true) {
				switch (t) {
					case Token::EndOfFile:
					case Token::Comma:
					case Token::CloseParen:
						scanner.decrement_position();
						goto outer;
					default:;
				}
				t = next_token();
			}
		}
		outer:
		location.length = scanner.position - start_position;
		diagnostics.push_back(create_diagnostic(location, D::Argument_must_be_a_variable_or_defined_by_a_preceding_comment));
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
	    u32string &localization_function_name_placeholder
	) {

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
		add_diagnostic(D::Expected_0_but_got_1,
		               token_enum_to_string.find(current_token)->second,
		               token_enum_to_string.find(expected_token)->second);
	    return false;
	}

	tuple<IdentifierType, bool> JavaScriptLocalizationExtractor::scan_type_and_or_identifier() {
		return scan_type_and_or_identifier(false);
	}

	tuple<IdentifierType, bool> JavaScriptLocalizationExtractor::scan_type_and_or_identifier(bool expect_type) {
		scan_expected(Token::Identifier);
		u32string type = get_value();
		if (is_of_supported_type(type)) {
			if (peek_next_token() == Token::OpenBracket) {
				next_token();
				scan_expected(Token::CloseBracket);
				type += U"[]";
			}
			IdentifierType identifier_type { make_shared<string>(to_utf8_string(type)), nullptr };
			return make_tuple(identifier_type, true);
		}
		else if (!expect_type && peek_next_token() == Token::Colon) {
			next_token();
			return scan_type_and_or_identifier(/*expect_type*/ true);
		}
		add_diagnostic(D::Unknown_type_definition_0, to_utf8_string(type));
		return make_tuple(IdentifierType {}, false);
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
		return t = scanner.next_token(false, false);
	}

	Token JavaScriptLocalizationExtractor::next_token(bool skip_whitespace, bool in_parameter_position) {
	    return t = scanner.next_token(skip_whitespace, in_parameter_position);
	}

	Token JavaScriptLocalizationExtractor::peek_next_token() {
		return peek_next_token(false, false);
	}

	Token JavaScriptLocalizationExtractor::peek_next_token(bool skip_whitespace, bool in_parameter_position) {
		scanner.save();
		const Token t = next_token(skip_whitespace, in_parameter_position);
		scanner.revert();
		return t;
	}

	u32string JavaScriptLocalizationExtractor::get_value() const {
	    return scanner.get_value();
	}

	string JavaScriptLocalizationExtractor::get_utf8_value() {
		return to_utf8_string(get_value());
	}

	SpanLocation JavaScriptLocalizationExtractor::get_token_location() const {
		return scanner.get_token_location();
	};

	string JavaScriptLocalizationExtractor::to_utf8_string(const u32string& str) {
		return scanner.to_utf8_string(str);
	}

	u32string JavaScriptLocalizationExtractor::to_u32_string(const string& str) {
		return scanner.to_u32_string(str);
	}

} // Lya::JavaScriptExtension
