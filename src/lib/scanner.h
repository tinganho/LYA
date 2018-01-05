
#ifndef SCANNER_H
#define SCANNER_H

#include <codecvt>
#include <locale>
#include <string>
#include <utils.h>
#include <types.h>
#include <memory>


using namespace std;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::lib {

	template<typename TToken>
	class Scanner {
	public:
	    Scanner(const u32string& text);
	    string to_utf8_string(const u32string& str);
	    u32string to_utf32_string(const string &str);
		u32string get_value() const;
		void save();
		void revert();
		void increment_position();
		void decrement_position();
		SpanLocation get_token_location() const;
		unsigned int position;
		unsigned int start_position;
		unsigned int end_position;
	    unsigned int line;
	    unsigned int column;
		unsigned int start_line;
		unsigned int start_column;
		virtual TToken next_token() = 0;
		std::unique_ptr<std::map<TToken, u32string>> token_enum_to_string;
		std::unique_ptr<std::map<u32string, TToken>> string_to_token_enum;

	protected:
		char32_t ch;
		unsigned int prev_line_length;
	    unsigned int length;
	    unsigned int saved_position;
		unsigned int saved_line;
		unsigned int saved_column;
	    unsigned int saved_start_position;
		unsigned int saved_end_position;
		unsigned int saved_start_line;
		unsigned int saved_start_column;
	    string value;
	    bool token_is_terminated;
	    u32string text;

		unsigned int get_length() const;
		char32_t curr_char() const;
		bool next_char_is(const char32_t& next_char);
		bool next_chars_are(const char32_t* chars);
		void scan_rest_of_line();
		void set_token_start_location();
		bool is_line_break(const char32_t& ch);
		bool is_identifier_start(const char32_t& ch);
		bool is_identifier_part(const char32_t& ch);
		void scan_string(char32_t quote);
		void scan_number();
		TToken peek_next_token();
	};


	template<typename TToken>
	Scanner<TToken>::Scanner(const u32string& _text):
		position(0),
		line(1),
		column(1),
		start_column(1),
		token_is_terminated(false),
		text(_text),
		token_enum_to_string(new map<TToken, u32string>()),
		string_to_token_enum(new map<u32string, TToken>()),
		length(_text.size())
	{ }

	template<typename TToken>
	TToken Scanner<TToken>::peek_next_token()
	{
		save();
		TToken t = next_token();
		revert();
	}

	template<typename TToken>
	void Scanner<TToken>::save()
	{
		saved_position = position;
		saved_column = column;
		saved_line = line;
		saved_start_position = start_position;
		saved_end_position = end_position;
		saved_start_line = start_line;
		saved_start_column = start_column;
	}

	template<typename TToken>
	void Scanner<TToken>::revert()
	{
		position = saved_position;
		column = saved_column;
		line = saved_line;
		start_position = saved_start_position;
		end_position = saved_end_position;
		start_line = saved_start_line;
		start_column = saved_start_column;
	}

	template<typename TToken>
	u32string Scanner<TToken>::get_value() const
	{
		auto t = text.substr(start_position, get_length());
		return t;
	}

	template<typename TToken>
	unsigned int Scanner<TToken>::get_length() const
	{
		return position - start_position;
	}

	template<typename TToken>
	SpanLocation Scanner<TToken>::get_token_location() const
	{
		return SpanLocation {
				start_line,
				start_column,
				get_length(),
				start_position,
		};
	}

	template<typename TToken>
	void Scanner<TToken>::set_token_start_location()
	{
		start_position = position;
		start_line = line;
		start_column = column;
	}

	template<typename TToken>
	void Scanner<TToken>::increment_position()
	{
		if (position >= length) {
			throw logic_error("You cannot increment more than max length.");
		}
		ch = text.at(position);
		if (ch == Character::LineFeed || ch == Character::CarriageReturn) {
			line++;
			prev_line_length = column;
			column = 1;
		}
		else {
			column++;
		}
		position++;
	}

	template<typename TToken>
	void Scanner<TToken>::decrement_position()
	{
		ch = text.at(--position);
		if (ch == Character::LineFeed || ch == Character::CarriageReturn) {
			line--;
			column = prev_line_length;
		}
		else {
			column--;
		}
	}

	template<typename TToken>
	bool Scanner<TToken>::is_line_break(const char32_t& ch)
	{
		return ch == LineFeed ||
		       ch == CarriageReturn ||
		       ch == LineSeparator ||
		       ch == ParagraphSeparator;
	}

	template<typename TToken>
	void Scanner<TToken>::scan_rest_of_line()
	{
		while (position < length && !is_line_break(curr_char())) {
			increment_position();
		}
	}

	template<typename TToken>
	bool Scanner<TToken>::is_identifier_start(const char32_t& ch)
	{
		return (ch >= A && ch <= Z) ||
		       (ch >= a && ch <= z) ||
		       ch == _;
	}

	template<typename TToken>
	char32_t Scanner<TToken>::curr_char() const
	{
		return text.at(position);
	}

	template<typename TToken>
	bool Scanner<TToken>::next_char_is(const char32_t& next_char)
	{
		increment_position();
		return curr_char() == next_char;
	}

	template<typename TToken>
	bool Scanner<TToken>::is_identifier_part(const char32_t& ch)
	{
		return (ch >= a && ch <= z) ||
		       (ch >= A && ch <= Z) ||
		       (ch >= _0 && ch <= _9) ||
		       ch == _;
	}

	template<typename TToken>
	void Scanner<TToken>::scan_string(char32_t quote)
	{
		ch = curr_char();
		while (true) {
			if (position >= length) {
				token_is_terminated = true;
				break;
			}
			if (ch == quote) {
				increment_position();
				break;
			}
			if (ch == Backslash) {
				increment_position();
				if (ch == quote) {
					increment_position();
				}
				continue;
			}
			if (is_line_break(ch)) {
				token_is_terminated = true;
				break;
			}
			increment_position();
			ch = curr_char();
		}
	}

	template<typename TToken>
	void Scanner<TToken>::scan_number()
	{
		ch = curr_char();
		bool has_dot = false;
		while (true) {
			if (position >= length) {
				token_is_terminated = true;
				break;
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
					break;
				case Character::Dot:
					if (has_dot) {
						return;
					}
					has_dot = true;
					continue;
				default:
					return;
			}
			increment_position();
			ch = curr_char();
		}
	}

	template<typename TToken>
	bool Scanner<TToken>::next_chars_are(const char32_t* chars)
	{
		bool successful_scan = true;
		ch = curr_char();
		while (*chars != '\0') {
			if (ch != *chars) {
				successful_scan = false;
			}
			increment_position();
			ch = curr_char();
			chars++;
		}
		return successful_scan;
	}

	template<typename TToken>
	string Scanner<TToken>::to_utf8_string(const u32string& str)
	{
		return wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(str);
	}

	template<typename TToken>
	u32string Scanner<TToken>::to_utf32_string(const string &str)
	{
		return wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
	}

} // Lya::Extension

#endif
