
#include "JavaScriptTokenScanner.h"

#include <Server/Scanner.h>
#include <Types.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Extension;

namespace Lya::JavaScriptExtension {

JavaScriptTokenScanner::JavaScriptTokenScanner(const string& _file):
    Scanner(_file) {
}

Token JavaScriptTokenScanner::next_token() {
	start = pos;
    while (pos < length) {
        ch = text.at(pos);
        switch (ch) {
	        case Char::Dot:
	            return Token::Dot;
            case Char::Slash:
	            ch = text.at(++pos);
                if (ch == Char::Asterisk) {
                    scan_rest_of_line();
                    return Token::MultiLineCommentLine;
                }
                else if (ch == Char::Slash) {
                    scan_rest_of_line();
                    return Token::SingleLineCommentLine;
                }
                continue;
            case Char::SingleQuote:
            case Char::DoubleQuote:
                scan_string(ch);
                return Token::String;
            case Char::Comma:
                return Token::Comma;
            case Char::OpenParen:
                return Token::OpenParen;
            case Char::CloseParen:
                return Token::CloseParen;
            default:
                if (is_identifier_start(ch)) {
                    pos++;
                    while (pos < length && is_identifier_part(text.at(pos))) pos++;
                    end = pos;
                    return Token::Identifier;
                }

        }
    }

    return Token::EndOfFile;
}

u32string JavaScriptTokenScanner::get_value() {
	return text.substr(start, end - start);
}

void JavaScriptTokenScanner::scan_rest_of_line() {
    while (is_line_break(ch)) {
        next_char();
    }
}

void JavaScriptTokenScanner::scan_string(char32_t quote) {
    next_char();
    start = pos;
    while (true) {
        if (pos >= end) {
            token_is_terminated = true;
            break;
        }
        if (ch == quote) {
            break;
        }
        if (ch == Char::Backslash) {
            next_char();
            if (ch == quote) {
                next_char();
            }
            continue;
        }
        if (is_line_break(ch)) {
            token_is_terminated = true;
            break;
        }
        next_char();
    }
}

bool JavaScriptTokenScanner::is_line_break(const char32_t& ch) {
    return ch == Char::LineFeed ||
        ch == Char::CarriageReturn ||
        ch == Char::LineSeparator ||
        ch == Char::ParagraphSeparator;
}

bool JavaScriptTokenScanner::is_identifier_start(const char32_t& ch) {
    return (ch >= Char::A && ch <= Char::Z) ||
        (ch >= Char::a && ch <= Char::z) ||
        ch == Char::_;
}

bool JavaScriptTokenScanner::is_identifier_part(const char32_t& ch) {
    return (ch >= Char::A && ch <= Char::Z) ||
        (ch >= Char::a && ch <= Char::z) ||
        (ch >= Char::_0 && ch <= Char::_9) ||
        ch == Char::_;
}

} // Lya::JavaScriptExtension