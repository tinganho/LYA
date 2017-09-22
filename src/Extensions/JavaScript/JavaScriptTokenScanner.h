
#ifndef JAVASCRIPT_TOKEN_SCANNER_H
#define JAVASCRIPT_TOKEN_SCANNER_H

#include <Server/Scanner.h>
#include <Types.h>

using namespace std;
using namespace Lya::Types;
using namespace Lya::Extension;

namespace Lya::JavaScriptExtension {

enum class Token {
    None,
    String,
    SingleLineCommentLine,
    MultiLineCommentLine,
    Dot,
    Comma,
    OpenParen,
    CloseParen,
    Identifier,
    EndOfFile,
};

class JavaScriptTokenScanner: public Scanner {
public:
    JavaScriptTokenScanner(const string& _file);
    Token next_token();
    u32string get_value();

private:
    void scan_rest_of_line();
    void scan_string(char32_t quote);
    bool is_line_break(const char32_t& ch);
    bool is_identifier_start(const char32_t& ch);
    bool is_identifier_part(const char32_t& ch);
};

} // Lya::JavaScriptExtension

#endif // JAVASCRIPT_TOKEN_SCANNER_H
