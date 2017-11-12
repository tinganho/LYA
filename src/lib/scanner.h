
#ifndef SCANNER_H
#define SCANNER_H

#include <codecvt>
#include <locale>
#include <string>
#include <utils.h>
#include <types.h>

using namespace std;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::lib {

	template<typename T>
	class Scanner {
	public:
	    Scanner(const u32string& text);
	    string to_utf8_string(const u32string& str);
	    u32string to_u32_string(const string& str);
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
		virtual T next_token() = 0;
		map<T, u32string> token_enum_to_string;
		map<u32string, T> string_to_token_enum;

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
		void scan_rest_of_line();
		void set_token_start_location();
		bool is_line_break(const char32_t& ch);
		bool is_identifier_start(const char32_t& ch);
		bool is_identifier_part(const char32_t& ch);
		void scan_string(char32_t quote);

	private:
	    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;
	};


	template<typename T>
	Scanner<T>::Scanner(const u32string& _text):
		position(0),
		line(1),
		column(1),
		length(0),
		start_column(1),
		token_is_terminated(false),
		text(_text),
		length(_text.size()),
		utf8_char32_converter() { }

	template<typename T>
	void Scanner<T>::save() {
		saved_position = position;
		saved_column = column;
		saved_line = line;
		saved_start_position = start_position;
		saved_end_position = end_position;
		saved_start_line = start_line;
		saved_start_column = start_column;
	}

	template<typename T>
	void Scanner<T>::revert() {
		position = saved_position;
		column = saved_column;
		line = saved_line;
		start_position = saved_start_position;
		end_position = saved_end_position;
		start_line = saved_start_line;
		start_column = saved_start_column;
	}

	template<typename T>
	u32string Scanner<T>::get_value() const {
		return text.substr(start_position, get_length());
	}

	template<typename T>
	unsigned int Scanner<T>::get_length() const {
		return position - start_position;
	}

	template<typename T>
	SpanLocation Scanner<T>::get_token_location() const {
		return SpanLocation {
				start_line,
				start_column,
				get_length(),
				start_position,
		};
	}

	template<typename T>
	void Scanner<T>::set_token_start_location() {
		start_position = position;
		start_line = line;
		start_column = column;
	}

	template<typename T>
	void Scanner<T>::increment_position() {
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

	template<typename T>
	void Scanner<T>::decrement_position() {
		ch = text.at(--position);
		if (ch == Character::LineFeed || ch == Character::CarriageReturn) {
			line--;
			column = prev_line_length;
		}
		else {
			column--;
		}
	}

	template<typename T>
	bool Scanner<T>::is_line_break(const char32_t& ch) {
		return ch == LineFeed ||
		       ch == CarriageReturn ||
		       ch == LineSeparator ||
		       ch == ParagraphSeparator;
	}

	template<typename T>
	void Scanner<T>::scan_rest_of_line() {
		while (position < length && !is_line_break(curr_char())) {
			increment_position();
		}
	}

	template<typename T>
	bool Scanner<T>::is_identifier_start(const char32_t& ch) {
		return (ch >= A && ch <= Z) ||
		       (ch >= a && ch <= z) ||
		       ch == _;
	}

	template<typename T>
	char32_t Scanner<T>::curr_char() const {
		return text.at(position);
	}


	template<typename T>
	bool Scanner<T>::is_identifier_part(const char32_t& ch) {
		return (ch >= A && ch <= Z) ||
		       (ch >= a && ch <= z) ||
		       (ch >= _0 && ch <= _9) ||
		       ch == _;
	}

	template<typename T>
	void Scanner<T>::scan_string(char32_t quote) {
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

	template<typename T>
	string Scanner<T>::to_utf8_string(const u32string& str) {
		return utf8_char32_converter.to_bytes(str);
	}

	template<typename T>
	u32string Scanner<T>::to_u32_string(const string& str) {
		return utf8_char32_converter.from_bytes(str);
	}

} // Lya::Extension

#endif
