
#include "Scanner.h"
#include <string>
#include <Utils.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Utils;

namespace Lya::Extension {

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
	start_column = column;
	saved_position = position;
	saved_start_position = start_position;
	saved_end_position = end_position;
	saved_start_line = start_line;
	saved_start_column = start_column;
}

void Scanner::revert() {
	position = saved_position;
	start_position = saved_start_position;
	end_position = saved_end_position;
	start_line = saved_start_line;
	start_column = saved_start_column;
}

unsigned int Scanner::increment_position() {
	column++;
	return ++position;
}

u32string Scanner::get_value() const {
	return text.substr(start_position, end_position - start_position);
}

Location Scanner::get_token_location() const {
	return Location { start_line, start_column };
}

void Scanner::set_token_start_location() {
	start_position = position;
	start_line = line;
	start_column = column;
}

char32_t Scanner::next_char() {
    ch = text.at(position);
    if (ch == Character::LineFeed || ch == Character::CarriageReturn) {
        line++;
        column = 1;
    }
    else {
        column++;
    }
    position++;
    return ch;
}

string Scanner::to_utf8_string(const u32string& str) {
    return utf8_char32_converter.to_bytes(str);
}

u32string Scanner::to_u32_string(const string& str) {
    return utf8_char32_converter.from_bytes(str);
}

} // Lya::Extension

