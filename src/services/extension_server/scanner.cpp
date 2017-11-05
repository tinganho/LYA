
#include "scanner.h"
#include <string>
#include <utils.h>

using namespace std;
using namespace Lya::types;
using namespace Lya::utils;

namespace Lya::services {

	Scanner::Scanner(const string& file) :
	    position(0),
	    line(1),
	    column(1),
	    length(0),
	    start_column(1),
	    token_is_terminated(false),
	    utf8_char32_converter() {

		string file_content = read_file(file);
	    text = to_u32_string(file_content);
	    length = text.size();
	}

	void Scanner::save() {
		saved_position = position;
		saved_column = column;
		saved_line = line;
		saved_start_position = start_position;
		saved_end_position = end_position;
		saved_start_line = start_line;
		saved_start_column = start_column;
	}

	void Scanner::revert() {
		position = saved_position;
		column = saved_column;
		line = saved_line;
		start_position = saved_start_position;
		end_position = saved_end_position;
		start_line = saved_start_line;
		start_column = saved_start_column;
	}

	u32string Scanner::get_value() const {
		return text.substr(start_position, get_length());
	}

	unsigned int Scanner::get_length() const {
		return position - start_position;
	}

	SpanLocation Scanner::get_token_location() const {
		return SpanLocation {
			start_line,
			start_column,
			get_length(),
			start_position,
		};
	}

	void Scanner::set_token_start_location() {
		start_position = position;
		start_line = line;
		start_column = column;
	}

	void Scanner::increment_position() {
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

	void Scanner::decrement_position() {
		ch = text.at(--position);
		if (ch == Character::LineFeed || ch == Character::CarriageReturn) {
			line--;
			column = prev_line_length;
		}
		else {
			column--;
		}
	}

	char32_t Scanner::curr_char() {
		return text.at(position);
	}

	string Scanner::to_utf8_string(const u32string& str) {
	    return utf8_char32_converter.to_bytes(str);
	}

	u32string Scanner::to_u32_string(const string& str) {
	    return utf8_char32_converter.from_bytes(str);
	}

} // Lya::Extension

