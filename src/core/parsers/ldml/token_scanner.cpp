//
// Created by Tingan Ho on 2017-11-13.
//

#include <glibmm/ustring.h>
#include "syntaxes.h"
#include "token_scanner.h"

namespace Lya::core::parsers::ldml {
	TokenScanner::TokenScanner(const Glib::ustring& text):
		Scanner<LdmlToken>(text)
	{ }

	LdmlToken TokenScanner::next_token() {
		set_token_start_location();
		while (position < size) {
			ch = current_char();
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
					return LdmlToken::IntegerLiteral;
				case Character::Percent:
					return LdmlToken::Percent;
                case Character::Exclamation:
                    if (next_char_is('=')) {
                        return LdmlToken::NotEqual;
                    }
                    return LdmlToken::Unknown;
				case Character::Equal:
					return LdmlToken::Equal;
				case Character::HorizontalEllipsis:
					return LdmlToken::HorizontalEllipsis;
				case Character::Tilde:
					return LdmlToken::Tilde;
				case Character::Comma:
					return LdmlToken::Comma;
				case Character::n:
					return LdmlToken::AbsoluteValueTransform;
				case Character::i:
					return LdmlToken::IntegerValueTransform;
				case Character::v:
					return LdmlToken::NumberOfVisibleFractionDigits_WithTrailingZeroTransform;
				case Character::w:
					return LdmlToken::NumberOfVisibleFractionDigits_WithoutTrailingZeroTransform;
				case Character::f:
					return LdmlToken::VisibleFractionDigits_WithTrailingZeroTransform;
				case Character::t:
					return LdmlToken::VisibleFractionDigits_WithoutTrailingZeroTransform;
				case Character::o:
					if (next_char_is('r')) {
						return LdmlToken::LogicalOr;
					}
					return LdmlToken::Unknown;

				case Character::a:
					if (next_chars_are(U"nd")) {
						return LdmlToken::LogicalAnd;
					}
					return LdmlToken::Unknown;
				case Character::At:
					increment_position();
					ch = current_char();

					switch (ch) {
						case Character::i: {
							if (next_chars_are(U"nteger")) {
								return LdmlToken::AtInteger;
							}
							return LdmlToken::Unknown;
						}
						case Character::d: {
							if (next_chars_are(U"ecimal")) {
								return LdmlToken::AtDecimal;
							}
							return LdmlToken::Unknown;
						}
					}
			}
		}
		throw logic_error("Should not reach here.");
	}
}
