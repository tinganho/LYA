//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_TOKEN_SCANNER_H
#define LYA_MESSAGE_TOKEN_SCANNER_H

#include <stack>
#include <glibmm/ustring.h>
#include "scanner.h"
#include "syntaxes.h"
#include "diagnostic.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {

	class TokenScanner final : public Scanner<MessageToken>, public Lya::lib::DiagnosticList<TokenScanner> {
	public:
		TokenScanner(const Glib::ustring& text);
		MessageToken next_token() override;
		MessageToken get_identifier_token(const Glib::ustring& value);
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
