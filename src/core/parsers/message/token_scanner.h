//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_TOKEN_SCANNER_H
#define LYA_MESSAGE_TOKEN_SCANNER_H

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
		AttributeKeyword,

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

	class TokenScanner : public Scanner<Token>, public Lya::lib::DiagnosticList<TokenScanner> {
	public:
		TokenScanner(const u32string& text);
		Token next_token() override;
		Token get_identifier_token(const u32string &value);
		void scan_text();
	private:
		// A property representing whether the current position is in a formatted text, inside braces
		// of any kind of complex message (InterpolationMessage, PluralMessage etc.).
		//
		// @example
		//   "{rabbits, plural, other {{name}, {sex, context, male {He} female {She}} has # rabbits}}
		//   ~ in_formatted_text == false
		//    ~ in_formatted_text = true
		//     ~~~~~~~~~~~~~~~~~~~~~~~ in_formatted_text == true
		//                            ~ in_formatted_text = false
		//                             ~ in_formatted_text = true
		//                              ~~~~ in_formatted_text == true
		//                                  ~ in_formatted_text = false
		//                                   ~~ in_formatted_text == false
		//                                     ~ in_formatted_text = true
		//                                      ~~~~~~~~~~~~~~~~~~~ in_formatted_text == true
		//                                                         ~ in_formatted_text = false
		//                                                          ~~ in_formatted_text == false
		//                                                            ~ in_formatted_text = true
		//                                                             ~~~~~~~ in_formatted_text == true
		//                                                                    ~ in_formatted_text = false
		//                                                                     ~~~ in_formatted_text == false
		//                                                                        ~ in_formatted_text = true
		//                                                                         ~ in_formatted_text = false
		//                                                                          ~~~~~~~~~~~~~~~~ in_formatted_text == false
		//                                                                                          ~ in_formatted_text = true
		//                                                                                           ~ in_formatted_text = false
		//                                                                                            ~ in_formatted_text == false
		bool in_formatted_text;
		unsigned int unmatched_braces;
	};
}

#endif // LYA_MESSAGE_TOKEN_SCANNER_H
