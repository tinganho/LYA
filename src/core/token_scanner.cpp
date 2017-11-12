//
// Created by Tingan Ho on 2017-11-06.
//

#include "token_scanner.h"
#include "diagnostics.h"
#include "utils.h"
#include "types.h"

using namespace Lya::core::diagnostics;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::core::message_parser {
	TokenScanner::TokenScanner(const u32string& text) : Scanner(text) {
		token_enum_to_string = {
				make_pair(Token::Comma, U"Comma"),
				make_pair(Token::OpenBrace, U"OpenBrace"),
				make_pair(Token::CloseBrace, U"CloseBrace"),
				make_pair(Token::Identifier, U"Identifier"),
		};
		string_to_token_enum = create_reverse_map<Token, u32string>(token_enum_to_string);
	}

	Token TokenScanner::next_token() {
		set_token_start_location();
		increment_position();
		while (position < length) {
			ch = curr_char();
			increment_position();

			switch (ch) {
				case Character::Comma:
					return Token::Comma;
				case Character::OpenBrace:
					return Token::OpenBrace;
				case Character::CloseBrace:
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
		throw;
	}

	Token TokenScanner::get_identifier_token(const u32string &value) {
		unsigned int size = value.size();

		// keywords are between 3 and 8 in size
		if (size >= 4 && size <= 8) {
			const char32_t ch = value.at(0);
			if (ch >= Character::a && ch <= Character::z) {
				auto it = string_to_token_enum.find(value);
				if (it != string_to_token_enum.end()) {
					return it->second;
				}
			}
		}
		return Token::Identifier;
	}
}