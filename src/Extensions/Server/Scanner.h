
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
    string to_utf8_string(const u32string& str);
    u32string to_u32_string(const string& str);
	void save();
	void revert();
	u32string get_value() const;
	Location get_token_location() const;
    unsigned int line;
    unsigned int column;
	unsigned int start_line;
	unsigned int start_column;

protected:
    unsigned int position;
    char32_t ch;
    unsigned int start_position;
    unsigned int end_position;
    unsigned int length;
    unsigned int saved_position;
    unsigned int saved_start_position;
	unsigned int saved_end_position;
	unsigned int saved_start_line;
	unsigned int saved_start_column;

    string value;
    bool token_is_terminated;
    u32string text;
	unsigned int increment_position();
	void scan_rest_of_line();
	void set_token_start_location();

private:
    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;
};

} // Lya::Extension

#endif
