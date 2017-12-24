//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_SCANNER_H
#define LYA_MESSAGE_SCANNER_H

#include <stack>
#include "scanner.h"
#include "diagnostic.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {
	enum class Token {
		Identifier,

		// Symbols
		OpenBrace,
		CloseBrace,
		Comma,
		Equals,

		Number,

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

		Text,
		EndOfFile,
	};

	class TokenScanner : public Scanner<Token>, public Lya::lib::Diagnostic<TokenScanner> {
	public:
		TokenScanner(const u32string& text);
		Token next_token() override;
		Token get_identifier_token(const u32string &value);
		void scan_text();
	private:
		// Each item in the vector represent a layer. Messages in the same layer
		// share the same flag. Though, a flag should be set to zero after each
		// message unless there is syntax error.
		//
		//       [Message]       Layer 1
		//        |     |
		//   [Message] [Message] Layer 2
		bool in_formatted_text;
	};
}

#endif //LYA_MESSAGE_SCANNER_H
