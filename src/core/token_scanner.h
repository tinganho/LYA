//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_SCANNER_H
#define LYA_MESSAGE_SCANNER_H

#include "scanner.h"
#include "diagnostic.h"

using namespace Lya::lib;

namespace Lya::core::message_parser {
	enum class Token {
		Identifier,

		// Symbols
		OpenBrace,
		CloseBrace,
		Comma,
		Equals,

		Integer,

		// Type keywords
		PluralKeyword,
		OrdinalKeyword,
		ContextKeyword,
		NumberKeyword,
		CurrencyKeyword,
		DateKeyword,
		ListKeyword,

		// Plural category keywords
		ZeroKeyword,
		OneKeyword,
		TwoKeyword,
		FewKeyword,
		ManyKeyword,
		OtherKeyword,

		EndOfFile,
	};

	class TokenScanner : public Scanner<Token>, public Lya::lib::Diagnostic<TokenScanner> {
	public:
		TokenScanner(const u32string& text);
		Token next_token() override;
		Token get_identifier_token(const u32string &value);
	};
}

#endif //LYA_MESSAGE_SCANNER_H
