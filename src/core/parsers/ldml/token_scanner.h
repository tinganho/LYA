
#ifndef LYA_TOKEN_SCANNER_H
#define LYA_TOKEN_SCANNER_H

#include "scanner.h"

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {

	enum class Token {

		Unknown,
		EndOfText,

		// Logic,
		LogicalAnd,
		LogicalOr,
		Equality,
		NotEqual,
		Remainder,
		Is,
		Not,

		IntegerLiteral,
		DoubleLiteral,

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
		NumberOfVisibleFractionDigits_WithTrailingZeroTransform, // v
		NumberOfVisibleFractionDigits_WithoutTrailingZeroTransform, // w
		VisibleFractionDigits_WithTrailingZeroTransform, // f
		VisibleFractionDigits_WithoutTrailingZeroTransform, // t

	};

	class TokenScanner : public Scanner<Token> {
	public:
		TokenScanner(const u32string& text);
		Token next_token() override;
	};
}


#endif //LYA_TOKEN_SCANNER_H
