
#ifndef JAVASCRIPT_TOKEN_SCANNER_H
#define JAVASCRIPT_TOKEN_SCANNER_H

#include "../../services/extension_server/scanner.h"
#include <types.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Extension;

namespace Lya::JavaScriptExtension {

enum class Token {
    None,

	// Expression Values
	Identifier,
    String,
	Number,
	Null,
	Undefined,
	True,
	False,

	Newline,

	BinaryOperator,

	SlashAsterix,
	AsterixSlash,
    MultiLineComment,
	SingleLineComment,
    Dot,
    Comma,
	Colon,
    OpenParen,
    CloseParen,
	OpenBrace,
	CloseBrace,
	OpenBracket,
	CloseBracket,
    EndOfFile,
	InvalidArgument,
	Trivia,
};

extern map<Token, string> token_enum_to_string;

class JavaScriptTokenScanner: public Scanner {
public:
    JavaScriptTokenScanner(const string& _file);
	Token next_token();
    Token next_token(bool skip_whitespace, bool in_parameter_position);

private:
	void scan_to_next_comma_or_close_paren();
	void scan_multiline_comment();
    void scan_rest_of_line();
    void scan_string(char32_t quote);
    bool is_line_break(const char32_t& ch);
    bool is_identifier_start(const char32_t& ch);
    bool is_identifier_part(const char32_t& ch);
};

} // Lya::JavaScriptExtension

#endif // JAVASCRIPT_TOKEN_SCANNER_H
