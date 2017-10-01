
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
    token_is_terminated(false),
    utf8_char32_converter() {

	string file_content = read_file(file);
    text = to_u32_string(file_content);
    length = text.size();
}

void Scanner::save() {
	saved_pos = position;
	saved_start = start;
	saved_end = end;
}

void Scanner::revert() {
	position = saved_pos;
	start = saved_start;
	end = saved_end;
}

unsigned int Scanner::increment_position() {
	column++;
	return ++position;
}

u32string Scanner::get_value() const {
	return text.substr(start, end - start);
}

Location Scanner::get_location() const {
	return Location { line, column };
}

char32_t Scanner::current_char() {
	return text.at(position);
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

