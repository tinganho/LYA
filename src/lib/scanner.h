
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

	class Scanner {
	public:
	    Scanner(const string& file);
	    void increment_position();
		void decrement_position();
		char32_t curr_char();
		char32_t prev_char();
	    string to_utf8_string(const u32string& str);
	    u32string to_u32_string(const string& str);
		void save();
		void revert();
		u32string get_value() const;
		SpanLocation get_token_location() const;
		unsigned int position;
		unsigned int start_position;
		unsigned int end_position;
	    unsigned int line;
	    unsigned int column;
		unsigned int start_line;
		unsigned int start_column;

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
		void scan_rest_of_line();
		void set_token_start_location();

	private:
	    wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8_char32_converter;
};

} // Lya::Extension

#endif
