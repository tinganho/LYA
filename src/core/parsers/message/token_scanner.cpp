//
// Created by Tingan Ho on 2017-11-06.
//

#include <glibmm/ustring.h>
#include <memory>
#include "token_scanner.h"
#include "syntaxes.h"
#include "diagnostics/diagnostics.h"

using namespace Lya::core::diagnostics;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::core::parsers::message {

	TokenScanner::TokenScanner(const Glib::ustring& text):
		Scanner<MessageToken>(text, ldml_token_enum_to_string, reverse_map(ldml_token_enum_to_string)),
		in_formatted_text(false),
		unmatched_braces(0)
	{ }

	MessageToken TokenScanner::next_token()
	{
		set_token_start_location();
		while (position < size)
		{
			ch = current_char();
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
				case Character::Equal:
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
						while (position < size && is_identifier_part(current_char()))
						{
							increment_position();
						}
						return get_identifier_token(get_token_value());
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
				ch = current_char();
				continue;
			}
			if (ch == Character::CloseBrace ||
				ch == Character::OpenBrace ||
				position == size)
			{
				break;
			}
			increment_position();
			ch = current_char();
		}
	}

	MessageToken TokenScanner::get_identifier_token(const Glib::ustring& value)
	{
        auto size = static_cast<unsigned int>(value.size());

		// keywords are between 3 and 8 in size
		if (size >= 3 && size <= 8)
		{
			const char32_t ch = value.at(0);
			if (ch >= Character::a && ch <= Character::z)
			{
				auto it = string_to_token_enum.find(value);
				if (it != string_to_token_enum.end())
				{
					return it->second;
				}
			}
		}
		return MessageToken::Identifier;
	}
}