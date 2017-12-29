//
// Created by Tingan Ho on 2017-11-06.
//

#include "token_scanner.h"
#include "diagnostics/diagnostics.h"

using namespace Lya::core::diagnostics;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::core::parsers::message {

	TokenScanner::TokenScanner(const u32string& text):
		Scanner<Token>(text),
		in_formatted_text(false),
		unmatched_braces(0)
	{
		token_enum_to_string->emplace(Token::PluralKeyword, U"plural");
		token_enum_to_string->emplace(Token::OrdinalKeyword, U"ordinal");
		token_enum_to_string->emplace(Token::ContextKeyword, U"context");
		token_enum_to_string->emplace(Token::NumberKeyword, U"number");
		token_enum_to_string->emplace(Token::CurrencyKeyword, U"currency");
		token_enum_to_string->emplace(Token::DateKeyword, U"date");
		token_enum_to_string->emplace(Token::ListKeyword, U"list");

		token_enum_to_string->emplace(Token::ZeroKeyword, U"zero");
		token_enum_to_string->emplace(Token::OneKeyword, U"one");
		token_enum_to_string->emplace(Token::TwoKeyword, U"two");
		token_enum_to_string->emplace(Token::ManyKeyword, U"many");
		token_enum_to_string->emplace(Token::OtherKeyword, U"other");
		create_reverse_map(*token_enum_to_string, *string_to_token_enum);
	}

	Token TokenScanner::next_token()
	{
		set_token_start_location();
		while (position < length)
		{
			ch = curr_char();
			increment_position();

			if (ch == Character::OpenBrace)
			{
				in_formatted_text = !in_formatted_text;
				unmatched_braces++;
				return Token::OpenBrace;
			}

			if (!in_formatted_text)
			{
				// We don't skip close brace character when we scan text.
				if (ch == Character::CloseBrace)
				{
					in_formatted_text = !in_formatted_text;
					unmatched_braces--;
					return Token::CloseBrace;
				}
				scan_text();
				return Token::Text;
			}

			switch (ch) {
				case Character::Space:
				case Character::CarriageReturn:
				case Character::LineFeed:
					set_token_start_location();
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
					return Token::Number;
				case Character::Equals:
					return Token::Equals;
				case Character::Comma:
					return Token::Comma;
				case Character::CloseBrace:
					in_formatted_text = !in_formatted_text;
					unmatched_braces--;
					return Token::CloseBrace;
				default:
					if (is_identifier_start(ch))
					{
						while (position < length && is_identifier_part(curr_char()))
						{
							increment_position();
						}
						return get_identifier_token(get_value());
					}
			}
		}
		return Token::EndOfFile;
	}

	void TokenScanner::scan_text()
	{
		while (ch != Character::OpenBrace)
		{
			if (ch == Character::Backslash)
			{
				increment_position();
				ch = curr_char();
				continue;
			}
			if (ch == Character::CloseBrace ||
				ch == Character::OpenBrace ||
				position == length)
			{
				break;
			}
			increment_position();
			ch = curr_char();
		}
	}

	Token TokenScanner::get_identifier_token(const u32string &value)
	{
		unsigned int size = value.size();

		// keywords are between 3 and 8 in size
		if (size >= 3 && size <= 8)
		{
			const char32_t ch = value.at(0);
			if (ch >= Character::a && ch <= Character::z)
			{
				auto it = string_to_token_enum->find(value);
				if (it != string_to_token_enum->end())
				{
					return it->second;
				}
			}
		}
		return Token::Identifier;
	}
}