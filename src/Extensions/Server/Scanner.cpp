
#include "Scanner.h"
#include <string>
#include <Utils.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Utils;

namespace Lya {
namespace Extension {

Scanner::Scanner(const string& file) :
    pos(0),
    line(1),
    column(1),
    length(0),
    token_is_terminated(false),
    utf8_char32_converter() {

    text = to_u32_string(read_file(file));
    length = text.size();
}

void Scanner::save() {
	saved_pos = pos;
	saved_start = start;
	saved_end = end;
}

void Scanner::revert() {
	pos = saved_pos;
	start = saved_start;
	end = saved_end;
}

char32_t Scanner::current_char() {
	return text.at(pos);
}

char32_t Scanner::next_char() {
    ch = text.at(++pos);
    if (ch == Char::LineFeed || ch == Char::CarriageReturn) {
        line++;
        column = 1;
    }
    else {
        column++;
    }
    return ch;
}

string Scanner::to_utf8_string(u32string str) {
    return utf8_char32_converter.to_bytes(str);
}

u32string Scanner::to_u32_string(string str) {
    return utf8_char32_converter.from_bytes(str);
}

} // Extension
} // Lya

