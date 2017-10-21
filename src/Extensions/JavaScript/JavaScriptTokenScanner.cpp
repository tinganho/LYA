
#include "JavaScriptTokenScanner.h"

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
		return next_token(false, false);
	}

	Token JavaScriptTokenScanner::next_token(bool skip_whitespace = false, bool in_parameter_position = false) {
		set_token_start_location();
	    while (position < length) {
	        ch = curr_char();
		    increment_position();
	        switch (ch) {
		        case Character::Dot:
		            return Token::Dot;
	            case Character::Slash:
		            ch = curr_char();
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
			        if (curr_char() == Character::Slash) {
				        increment_position();
				        return Token::AsterixSlash;
			        }
		        case Character::Plus:
		        case Character::Minus:
		        case Character::Equals:
			        return Token::BinaryOperator;

	            default:
	                if (is_identifier_start(ch)) {
	                    while (position < length && is_identifier_part(curr_char())) {
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
			if (position >= length) {
				token_is_terminated = true;
				break;
			}
			ch = curr_char();
			switch (ch) {
				case Character::Slash:
					if (curr_char() == Character::Asterisk) {
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
			if (position >= length) {
				token_is_terminated = true;
				break;
			}
			if (ch == Character::Asterisk) {
				increment_position();
				if (curr_char() == Character::Slash) {
					break;
				}
			}
			increment_position();
			ch = curr_char();
		}
	}

	void JavaScriptTokenScanner::scan_rest_of_line() {
	    while (position < length && !is_line_break(curr_char())) {
		    increment_position();
	    }
	}

	void JavaScriptTokenScanner::scan_string(char32_t quote) {
		ch = curr_char();
	    while (true) {
	        if (position >= length) {
	            token_is_terminated = true;
	            break;
	        }
	        if (ch == quote) {
		        increment_position();
	            break;
	        }
	        if (ch == Character::Backslash) {
		        increment_position();
	            if (ch == quote) {
		            increment_position();
	            }
	            continue;
	        }
	        if (is_line_break(ch)) {
	            token_is_terminated = true;
	            break;
	        }
		    increment_position();
		    ch = curr_char();
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