
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

namespace Lya::Extension {

class Scanner {
public:
    Scanner(const string& file);
    char32_t next_char();
	char32_t current_char();
    string to_utf8_string(const u32string& str);
    u32string to_u32_string(const string& str);
	void save();
	void revert();
	u32string get_value() const;
	Location get_location() const;
    unsigned int line;
    unsigned int column;

protected:
    unsigned int position;
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
	unsigned int increment_position();
	void scan_rest_of_line();

private:
    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;
};

} // Lya::Extension

#endif
