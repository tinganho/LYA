//
// Created by Tingan Ho on 2017-11-06.
//

#include "token_scanner.h"
#include "diagnostics/diagnostics.h"
#include <memory>
#include "utils.h"
#include "types.h"

using namespace Lya::core::diagnostics;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::core::parsers::message {

	TokenScanner::TokenScanner(const u32string& text):
		Scanner<Token>(text),
		in_formatted_text(false)
	{
		token_enum_to_string->emplace(Token::PluralKeyword, U"PluralKeyword");
		token_enum_to_string->emplace(Token::OrdinalKeyword, U"OrdinalKeyword");
		token_enum_to_string->emplace(Token::ContextKeyword, U"ContextKeyword");
		token_enum_to_string->emplace(Token::NumberKeyword, U"NumberKeyword");
		token_enum_to_string->emplace(Token::CurrencyKeyword, U"CurrencyKeyword");
		token_enum_to_string->emplace(Token::DateKeyword, U"DateKeyword");
		token_enum_to_string->emplace(Token::ListKeyword, U"ListKeyword");
		create_reverse_map(*token_enum_to_string, *string_to_token_enum);
	}

	Token TokenScanner::next_token()
	{
		set_token_start_location();
		increment_position();
		while (position < length) {
			ch = curr_char();
			increment_position();

			if (!in_formatted_text) {
				scan_text();
				return Token::Text;
			}

			switch (ch) {
				case Character::_0:
				case Character::_1:
				case Character::_2:
				case Character::_3:
				case Character::_4:
				case Character::_5:
				case Character::_6:
				case Character::_7:
				case Character::_8:
				case Character::_9:
					scan_number();
					return Token::Number;

				case Character::Equals:
					return Token::Equals;
				case Character::Comma:
					return Token::Comma;
				case Character::OpenBrace:
					in_formatted_text = !in_formatted_text;
					return Token::OpenBrace;
				case Character::CloseBrace:
					in_formatted_text = !in_formatted_text;
					return Token::CloseBrace;
				default:
					if (is_identifier_start(ch)) {
						while (position < length && is_identifier_part(curr_char())) {
							increment_position();
						}
						return get_identifier_token(get_value());
					}
			}
		}
		throw runtime_error("Could not parse token.");
	}

	void TokenScanner::scan_text()
	{
		while (ch != Character::OpenBrace) {
			if (ch == Character::Backslash) {
				increment_position();
				continue;
			}
			if (ch == Character::OpenBrace) {
				break;
			}
			ch = curr_char();
			increment_position();
		}
	}

	Token TokenScanner::get_identifier_token(const u32string &value)
	{
		unsigned int size = value.size();

		// keywords are between 3 and 8 in size
		if (size >= 4 && size <= 8) {
			const char32_t ch = value.at(0);
			if (ch >= Character::a && ch <= Character::z) {
				auto it = string_to_token_enum->find(value);
				if (it != string_to_token_enum->end()) {
					return it->second;
				}
			}
		}
		return Token::Identifier;
	}
}