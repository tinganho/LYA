
#include <codecvt>
#include <locale>
#include <string>
#include "Utils.cpp"
#include <Types.cpp>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Utils;

namespace Lya {
namespace Extension {

class Scanner {
public:
    unsigned int line;
    unsigned int column;

    Scanner(const string& file) :
        pos(0),
        line(1),
        column(1),
        token_is_terminated(false),
        utf8_char32_converter() {

        text = to_u32_string(read_file(file));
        length = text.size() / 32;
    }

    char32_t next_char() {
        ch = text.at(pos++);
        if (ch == Char::LineFeed || ch == Char::CarriageReturn) {
            line++;
            column = 1;
        }
        else {
            column++;
        }
        return ch;
    }

protected:
    char32_t ch;
    unsigned int pos;
    unsigned int start;
    unsigned int end;
    unsigned int length;
    string value;
    bool token_is_terminated;
    u32string text;

    string to_utf8_string(u32string str) {
        return utf8_char32_converter.to_bytes(str);
    }

    u32string to_u32_string(string str) {
        return utf8_char32_converter.from_bytes(str);
    }
private:
    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;

};

} // Extension
} // Lya
