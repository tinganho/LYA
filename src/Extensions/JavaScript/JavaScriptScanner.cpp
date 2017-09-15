#include <Server/Scanner.cpp>
#include <Types.cpp>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Extension;

namespace JavaScriptExtension {

enum class Token {
    EndOfFile,
    String,
    SingleLineCommentLine,
    MultiLineCommentLine,
    Comma,
    OpenParen,
    CloseParen,
    LocalizationFunction,
};

class JavaScriptScanner : public Scanner {
public:
    JavaScriptScanner(const string& _file, const vector<u32string> _function_names) :
        Scanner(_file),
        function_names(_function_names) { }

    Token next_token() {
        char32_t ch;

        while (pos < end) {
            next_char();
            switch (ch) {
                case Char::Slash:
                    next_char();
                    if (ch == Char::Slash) {
                        next_char();
                        if (ch == Char::Asterisk) {
                            return Token::MultiLineCommentLine;
                        }
                        else {

                        }
                        return Token::SingleLineCommentLine;
                    }
                case Char::SingleQuote:
                case Char::DoubleQuote:
                    value = scan_string(ch);
                    return Token::String;
                case Char::Comma:
                    return Token::Comma;
                case Char::OpenParen:
                    return Token::OpenParen;
                case Char::CloseParen:
                    return Token::CloseParen;
                default:
                    if (is_localization_function_start(ch)) {
                        start = pos;
                        pos++;
                        while (pos < end && is_localization_function_part(ch)) {
                            pos++;
                        }
                        end = pos;
                        for (const u32string& fn : function_names) {
                            if (text.substr(start, end) == fn) {
                                return Token::LocalizationFunction;
                            }
                        }
                    }
            }
        }

        return Token::EndOfFile;
    }

private:

    const vector<u32string>& function_names;

    string scan_string(char32_t quote) {
        next_char();
        string result = "";
        start = pos;
        while (true) {
            if (pos >= end) {
                result += to_utf8_string(text.substr(start, pos - start));
                token_is_terminated = true;
                break;
            }
            if (ch == quote) {
                result += to_utf8_string(text.substr(start, pos - start));
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
                result += to_utf8_string(text.substr(start, pos - start));
                token_is_terminated = true;
                break;
            }
            next_char();
        }
        return result;
    }

    bool is_line_break(const char32_t& ch) {
        return ch == Char::LineFeed ||
            ch == Char::CarriageReturn ||
            ch == Char::LineSeparator ||
            ch == Char::ParagraphSeparator;
    }

    bool is_localization_function_start(const char32_t& ch) {
        return (ch >= Char::A && ch <= Char::Z) ||
            (ch >= Char::a && ch <= Char::z) ||
            ch == Char::_;
    }

    bool is_localization_function_part(const char32_t& ch) {
        return (ch >= Char::A && ch <= Char::Z) ||
            (ch >= Char::a && ch <= Char::z) ||
            (ch >= Char::_0 && ch <= Char::_9) ||
            ch == Char::Dot ||
            ch == Char::_;
    }
};

} // JavaScriptExtension
