//
// Created by Tingan Ho on 2017-11-06.
//

#include <memory>
#include "token_scanner.h"
#include "syntaxes.h"
#include "diagnostics/diagnostics.h"

using namespace Lya::core::diagnostics;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::core::parsers::message {

	TokenScanner::TokenScanner(const u32string& text):
		Scanner<MessageToken>(text),
		in_formatted_text(false),
		unmatched_braces(0)
	{
		token_enum_to_string = std::make_unique<std::map<MessageToken, std::u32string>>(ldml_token_enum_to_string);
		std::unique_ptr<std::map<std::u32string, MessageToken>> tmp_map = std::make_unique<std::map<std::u32string, MessageToken>>();
		create_reverse_map(*token_enum_to_string, *tmp_map);
		string_to_token_enum = std::move(tmp_map);
	}

	MessageToken TokenScanner::next_token()
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
				return MessageToken::OpenBrace;
			}

			if (!in_formatted_text)
			{
				// We don't skip close brace character when we scan text.
				if (ch == Character::CloseBrace)
				{
					in_formatted_text = !in_formatted_text;
					unmatched_braces--;
					return MessageToken::CloseBrace;
				}
				scan_text();
				return MessageToken::Text;
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
					return MessageToken::Number;
				case Character::Equals:
					return MessageToken::Equals;
				case Character::Comma:
					return MessageToken::Comma;
				case Character::CloseBrace:
					in_formatted_text = !in_formatted_text;
					unmatched_braces--;
					return MessageToken::CloseBrace;
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
		return MessageToken::EndOfFile;
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

	MessageToken TokenScanner::get_identifier_token(const u32string &value)
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
		return MessageToken::Identifier;
	}
}