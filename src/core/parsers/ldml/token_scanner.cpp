//
// Created by Tingan Ho on 2017-11-13.
//

#include "token_scanner.h"

namespace Lya::core::parsers::ldml {
	TokenScanner::TokenScanner(const u32string& text):
		Scanner<Token>(text)
	{ }

	Token TokenScanner::next_token() {
		set_token_start_location();
		while (position < length) {
			ch = curr_char();
			increment_position();

			switch (ch) {
				case Character::Space:
					continue;
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
					return Token::Integer;
				case Character::Percent:
					return Token::Percent;
				case Character::Equals:
					return Token::Equal;
				case Character::HorizontalEllipsis:
					return Token::HorizontalEllipsis;
				case Character::Tilde:
					return Token::Tilde;
				case Character::Comma:
					return Token::Comma;
				case Character::n:
					return Token::AbsoluteValueTransform;
				case Character::i:
					return Token::IntegerValueTransform;
				case Character::v:
					return Token::NumberOfVisibleFractionDigits_WithTrailingZero;
				case Character::w:
					return Token::NumberOfVisibleFractionDigits_WithoutTrailingZero;
				case Character::f:
					return Token::VisibleFractionDigits_WithTrailingZero;
				case Character::t:
					return Token::VisibleFractionDigits_WithoutTrailingZero;
				case Character::o:
					if (next_char_is('r')) {
						return Token::Or;
					}
					return Token::Unknown;

				case Character::a:
					if (next_chars_are(U"nd")) {
						return Token::And;
					}
					return Token::Unknown;
				case Character::At:
					increment_position();
					ch = curr_char();

					switch (ch) {
						case Character::i: {
							if (next_chars_are(U"nteger")) {
								return Token::AtInteger;
							}
							return Token::Unknown;
						}
						case Character::d: {
							if (next_chars_are(U"ecimal")) {
								return Token::AtDecimal;
							}
							return Token::Unknown;
						}
					}
			}
		}
	}
}
