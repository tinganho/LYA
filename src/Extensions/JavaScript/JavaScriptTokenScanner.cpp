
#include "JavaScriptTokenScanner.h"

#include <Server/Scanner.h>
#include <Types.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Extension;

namespace Lya::JavaScriptExtension {

map<Token, string> token_enum_to_string = {
	make_pair(Token::None, "None"),
	make_pair(Token::String, "String"),
	make_pair(Token::SlashAsterix, "SlashAsterix"),
	make_pair(Token::AsterixSlash, "AsterixSlash"),
	make_pair(Token::MultiLineComment, "MultiLineComment"),
	make_pair(Token::SingleLineComment, "SingleLineComment"),
	make_pair(Token::Dot, "Dot"),
	make_pair(Token::Comma, "Comma"),
	make_pair(Token::OpenParen, "OpenParen"),
	make_pair(Token::CloseParen, "CloseParen"),
	make_pair(Token::OpenBrace, "OpenBrace"),
	make_pair(Token::CloseBrace, "CloseBrace"),
	make_pair(Token::OpenBracket, "OpenBracket"),
	make_pair(Token::CloseBracket, "CloseBracket"),
	make_pair(Token::None, "none"),
	make_pair(Token::Identifier, "Identifier"),
	make_pair(Token::Trivia, "Unknown"),
};

JavaScriptTokenScanner::JavaScriptTokenScanner(const string& _file):
    Scanner(_file) {
}

Token JavaScriptTokenScanner::next_token() {
	set_token_start_location();
    while (position < length) {
        ch = text.at(position);
        switch (ch) {
	        case Character::Dot:
		        increment_position();
	            return Token::Dot;
            case Character::Slash:
	            ch = text.at(++position);
                if (ch == Character::Asterisk) {
	                increment_position();
                    return Token::SlashAsterix;
                }
                else if (ch == Character::Slash) {
	                increment_position();
                    return Token::AsterixSlash;
                }
                continue;

	        case Character::FormFeed:
	        case Character::Space:
	        case Character::Tab:
	        case Character::VerticalTab:
		        increment_position();
		        set_token_start_location();
		        continue;

	        case Character::LineFeed:
	        case Character::CarriageReturn:
		        line++;
		        column = 1;
		        position++;
		        set_token_start_location();
		        continue;

            case Character::SingleQuote:
            case Character::DoubleQuote:
                scan_string(ch);
                return Token::String;
            case Character::Comma:
	            increment_position();
                return Token::Comma;
	        case Character::Colon:
		        increment_position();
	            return Token::Colon;
            case Character::OpenParen:
	            increment_position();
                return Token::OpenParen;
            case Character::CloseParen:
	            increment_position();
                return Token::CloseParen;
	        case Character::OpenBracket:
		        increment_position();
		        return Token::OpenBracket;
	        case Character::CloseBracket:
		        increment_position();
		        return Token::CloseBracket;
            default:
                if (is_identifier_start(ch)) {
	                increment_position();
                    while (position < length && is_identifier_part(text.at(position))) {
	                    increment_position();
                    }
                    end_position = position;
                    return Token::Identifier;
                }
		        increment_position();
		        set_token_start_location();
        }
    }

    return Token::EndOfFile;
}

void JavaScriptTokenScanner::scan_rest_of_line() {
    while (is_line_break(ch)) {
        next_char();
    }
}

void JavaScriptTokenScanner::scan_string(char32_t quote) {
    next_char();
    start_position = position;
    while (true) {
        if (position >= length) {
            token_is_terminated = true;
            break;
        }
        if (ch == quote) {
            break;
        }
        if (ch == Character::Backslash) {
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
    return ch == Character::LineFeed ||
        ch == Character::CarriageReturn ||
        ch == Character::LineSeparator ||
        ch == Character::ParagraphSeparator;
}

bool JavaScriptTokenScanner::is_identifier_start(const char32_t& ch) {
    return (ch >= Character::A && ch <= Character::Z) ||
        (ch >= Character::a && ch <= Character::z) ||
        ch == Character::_;
}

bool JavaScriptTokenScanner::is_identifier_part(const char32_t& ch) {
    return (ch >= Character::A && ch <= Character::Z) ||
        (ch >= Character::a && ch <= Character::z) ||
        (ch >= Character::_0 && ch <= Character::_9) ||
        ch == Character::_;
}

} // Lya::JavaScriptExtension