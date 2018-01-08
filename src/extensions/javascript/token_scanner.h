
#ifndef JAVASCRIPT_TOKEN_SCANNER_H
#define JAVASCRIPT_TOKEN_SCANNER_H

#include <glibmm/ustring.h>
#include "scanner.h"
#include "types.h"

using namespace std;
using namespace Lya::lib;

namespace Lya::javascript_extension {

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

	class JavaScriptTokenScanner: public Scanner<Token> {
	public:
	    JavaScriptTokenScanner(const Glib::ustring& _text);
		Token next_token();
	    Token next_token(bool skip_whitespace, bool in_parameter_position);

	private:
		void scan_to_next_comma_or_close_paren();
		void scan_multiline_comment();
	};

} // Lya::JavaScriptExtension

#endif // JAVASCRIPT_TOKEN_SCANNER_H
