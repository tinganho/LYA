
#ifndef SCANNER_H
#define SCANNER_H

#include <codecvt>
#include <locale>
#include <string>
#include <Utils.h>
#include <Types.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Utils;

namespace Lya {
namespace Extension {

class Scanner {
public:
    Scanner(const string& file);
    char32_t next_char();
    string to_utf8_string(u32string str);
    u32string to_u32_string(string str);
	void save();
	void revert();
    unsigned int line;
    unsigned int column;

protected:
    unsigned int pos;
    char32_t ch;
    unsigned int start;
    unsigned int end;
    unsigned int length;
    unsigned int saved_pos;
    unsigned int saved_start;
	unsigned int saved_end;
    string value;
    bool token_is_terminated;
    u32string text;

private:
    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;
	char32_t current_char();
};

} // Extension
} // Lya

#endif
