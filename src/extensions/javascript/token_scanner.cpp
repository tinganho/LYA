
#include "token_scanner.h"

using namespace std;
using namespace Lya::lib;

namespace Lya::javascript_extension {

	map<Token, string> token_enum_to_string = {
        { Token::None, "None" },
        { Token::String, "String" },
        { Token::SlashAsterix, "SlashAsterix" },
        { Token::AsterixSlash, "AsterixSlash" },
        { Token::MultiLineComment, "MultiLineComment" },
        { Token::SingleLineComment, "SingleLineComment" },
        { Token::Dot, "Dot" },
        { Token::Comma, "Comma" },
        { Token::OpenParen, "OpenParen" },
        { Token::CloseParen, "CloseParen" },
        { Token::OpenBrace, "OpenBrace" },
		{ Token::CloseBrace, "CloseBrace" },
		{ Token::OpenBracket, "OpenBracket" },
		{ Token::CloseBracket, "CloseBracket" },
		{ Token::None, "none" },
		{ Token::Identifier, "Identifier" },
		{ Token::Trivia, "Unknown" },
	};

	JavaScriptTokenScanner::JavaScriptTokenScanner(const Glib::ustring& text):
	    Scanner(text) {
	}

	Token JavaScriptTokenScanner::next_token() {
		return next_token(false, false);
	}

	Token JavaScriptTokenScanner::next_token(bool skip_whitespace = false, bool in_parameter_position = false) {
		set_token_start_location();
	    while (position < size) {
	        ch = current_char();
		    increment_position();
	        switch (ch) {
		        case Character::Dot:
		            return Token::Dot;
	            case Character::Slash:
		            ch = current_char();
	                if (ch == Character::Asterisk) {
		                increment_position();
	                    return Token::SlashAsterix;
	                }
	                else if (ch == Character::Slash) {
		                scan_rest_of_line();
	                    return Token::SingleLineComment;
	                }
			        return Token::BinaryOperator;

	            case Character::SingleQuote:
	            case Character::DoubleQuote:
		            scan_string(ch);
			        return Token::String;
	            case Character::Comma:
	                return Token::Comma;
		        case Character::Colon:
		            return Token::Colon;
	            case Character::OpenParen:
	                return Token::OpenParen;
	            case Character::CloseParen:
	                return Token::CloseParen;
		        case Character::OpenBracket:
			        return Token::OpenBracket;
		        case Character::CloseBracket:
			        return Token::CloseBracket;

		        case Character::FormFeed:
		        case Character::Space:
		        case Character::Tab:
		        case Character::VerticalTab:
			        set_token_start_location();
			        break;

		        case Character::LineFeed:
		        case Character::CarriageReturn:
			        if (skip_whitespace) {
						break;
			        }
			        return Token::Newline;

		        case Character::Asterisk:
			        if (current_char() == Character::Slash) {
				        increment_position();
				        return Token::AsterixSlash;
			        }
		        case Character::Plus:
		        case Character::Minus:
		        case Character::Equal:
			        return Token::BinaryOperator;

	            default:
	                if (is_identifier_start(ch)) {
	                    while (position < size && is_identifier_part(current_char())) {
		                    increment_position();
	                    }
	                    return Token::Identifier;
	                }
			        if (in_parameter_position) {
				        scan_to_next_comma_or_close_paren();
				        return Token::InvalidArgument;
			        }
			        set_token_start_location();
	        }
	    }

	    return Token::EndOfFile;
	}

	void JavaScriptTokenScanner::scan_to_next_comma_or_close_paren() {
		while (true) {
			if (position >= size) {
				token_is_terminated = true;
				break;
			}
			ch = current_char();
			switch (ch) {
				case Character::Slash:
					if (current_char() == Character::Asterisk) {
						scan_multiline_comment();
					}
					break;
				case Character::SingleQuote:
				case Character::DoubleQuote:
					scan_string(ch);
					break;
				case Character::Comma:
				case Character::CloseParen:
					goto outer;
				default:;
			}
			increment_position();
		}
		outer:;
	}

	void JavaScriptTokenScanner::scan_multiline_comment() {
		while (true) {
			if (position >= size) {
				token_is_terminated = true;
				break;
			}
			if (ch == Character::Asterisk) {
				increment_position();
				if (current_char() == Character::Slash) {
					break;
				}
			}
			increment_position();
			ch = current_char();
		}
	}

} // Lya::JavaScriptExtension