
#ifndef SCANNER_H
#define SCANNER_H

#include <glibmm/ustring.h>
#include <locale>
#include <string>
#include <utils.h>
#include <types.h>
#include <memory>

using namespace std;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;

namespace Lya::lib {

    struct SavedTextCursor {
        unsigned int position;
        unsigned int line;
        unsigned int column;
        unsigned int start_position;
        unsigned int end_position;
        unsigned int start_line;
        unsigned int start_column;
    };

	template<typename TToken>
	class Scanner {
	public:
        Scanner(const Glib::ustring& text);
	    Scanner(
            const Glib::ustring& text,
            std::map<TToken, Glib::ustring> token_enum_to_string,
            std::map<Glib::ustring, TToken> string_to_token_enum);
		Glib::ustring get_token_value() const;
		void save();
        void dispose_last_save();
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
		const std::map<TToken, Glib::ustring> token_enum_to_string;
		const std::map<Glib::ustring, TToken> string_to_token_enum;

	protected:
		char32_t ch;
		unsigned int prev_line_length;
	    unsigned int size;
	    Glib::ustring value;
	    bool token_is_terminated;
	    Glib::ustring text;
        std::stack<SavedTextCursor> saved_text_cursors;
		unsigned int get_length() const;
		char32_t current_char() const;
		bool next_char_is(const char32_t& next_char);
		bool next_chars_are(const char32_t* chars);
		void scan_rest_of_line();
		void set_token_start_location();
		bool is_line_break(const char32_t& ch);
		bool is_identifier_start(const char32_t& ch);
		bool is_identifier_part(const char32_t& ch);
		void scan_string(char32_t quote);
		void scan_number();
        char32_t peek_next_char();
		TToken peek_next_token();
	};


    template<typename TToken>
    Scanner<TToken>::Scanner(const Glib::ustring& text):
        Scanner(text, {}, {})
    { }

	template<typename TToken>
	Scanner<TToken>::Scanner(
        const Glib::ustring& text,
        std::map<TToken, Glib::ustring> token_enum_to_string,
        std::map<Glib::ustring, TToken> string_to_token_enum):

        position(0),
        line(1),
        column(1),
        start_column(1),
        token_is_terminated(false),
        text(text),
        size(static_cast<unsigned int>(text.size())),
        token_enum_to_string(token_enum_to_string),
        string_to_token_enum(string_to_token_enum)
	{ }

	template<typename TToken>
	TToken Scanner<TToken>::peek_next_token()
	{
		save();
		TToken t = next_token();
		revert();
	}

    template<typename TToken>
    char32_t Scanner<TToken>::peek_next_char()
    {
        char32_t ch;
        save();
        increment_position();
        ch = current_char();
        revert();
        return ch;
    }

	template<typename TToken>
	void Scanner<TToken>::save()
	{
        SavedTextCursor saved_text_cursor {
            position,
            column,
            line,
            start_position,
            end_position,
            start_line,
            start_column
        };
        saved_text_cursors.push(saved_text_cursor);
	}

	template<typename TToken>
	void Scanner<TToken>::revert()
	{
        const SavedTextCursor& saved_text_cursor = saved_text_cursors.top();
		position = saved_text_cursor.position;
		column = saved_text_cursor.column;
		line = saved_text_cursor.line;
		start_position = saved_text_cursor.start_position;
		end_position = saved_text_cursor.end_position;
		start_line = saved_text_cursor.start_line;
		start_column = saved_text_cursor.start_column;
        saved_text_cursors.pop();
	}

    template<typename TToken>
    void Scanner<TToken>::dispose_last_save()
    {
        saved_text_cursors.pop();
    }

	template<typename TToken>
	Glib::ustring Scanner<TToken>::get_token_value() const
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
		if (position >= size) {
			throw logic_error("You cannot increment more than max size.");
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
		while (position < size && !is_line_break(current_char())) {
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
	char32_t Scanner<TToken>::current_char() const
	{
		return text.at(position);
	}

	template<typename TToken>
	bool Scanner<TToken>::next_char_is(const char32_t& next_char)
	{
		if (peek_next_char() == next_char) {
            increment_position();
            return true;
        }
        return false;
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
		ch = current_char();
		while (true) {
			if (position >= size) {
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
			ch = current_char();
		}
	}

	template<typename TToken>
	void Scanner<TToken>::scan_number()
	{
		ch = current_char();
		bool has_dot = false;
		while (true) {
			if (position >= size) {
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
			ch = current_char();
		}
	}

	template<typename TToken>
	bool Scanner<TToken>::next_chars_are(const char32_t* chars)
	{
		bool successful_scan = true;
        save();
		ch = current_char();
		while (*chars != '\0') {
			if (ch != *chars) {
				successful_scan = false;
			}
			increment_position();
			ch = current_char();
			chars++;
		}
        if (!successful_scan) {
            revert();
        }
        else {
            dispose_last_save();
        }
		return successful_scan;
	}
} // Lya::Extension

#endif
