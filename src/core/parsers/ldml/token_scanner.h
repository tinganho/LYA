
#ifndef LYA_TOKEN_SCANNER_H
#define LYA_TOKEN_SCANNER_H

#include "scanner.h"

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {

	enum class Token {
		And,
		Or,
		Is,
		Not,
		Number,
		Range,
		IntegerSampleList,
		DecimalSampleList,
		AbsoluteOperand,
		IntegerOperand,
		Ellipses,
		NumberOfVisibleFractionDigits_WithTrailingZero,
		NumberOfVisibleFractionDigits_WithoutTrailingZero,
		VisibleFractionDigits_WithTrailingZero,
		VisibleFractionDigits_WithoutTrailingZero,
	};

	class TokenScanner : public Scanner<Token> {
		TokenScanner(const u32string& text);
		Token next_token() override;
	};
}


#endif //LYA_TOKEN_SCANNER_H
