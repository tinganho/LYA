
#ifndef LYA_TOKEN_SCANNER_H
#define LYA_TOKEN_SCANNER_H

#include "scanner.h"

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {

	enum class Token {

		Unknown,
		EndOfText,

		// Logic,
		And,
		Or,
		Equal,
		NotEqual,
		Modulo,
		Is,
		Not,

		Integer,
		Decimal,

		Range,

		// Samples
		AtInteger,
		AtDecimal,

		// Punctuations
		Comma,
		HorizontalEllipsis,
		Percent,
		Tilde,

		// Operands
		AbsoluteValueTransform, // n
		IntegerValueTransform, // i
		NumberOfVisibleFractionDigits_WithTrailingZero, // v
		NumberOfVisibleFractionDigits_WithoutTrailingZero, // w
		VisibleFractionDigits_WithTrailingZero, // f
		VisibleFractionDigits_WithoutTrailingZero, // t

	};

	class TokenScanner : public Scanner<Token> {
	public:
		TokenScanner(const u32string& text);
		Token next_token() override;
	};
}


#endif //LYA_TOKEN_SCANNER_H
