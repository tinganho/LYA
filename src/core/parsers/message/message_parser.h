//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_PARSER_H
#define LYA_MESSAGE_PARSER_H

#include "syntaxes.h"
#include <string>
#include "token_scanner.h"
#include "parser.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {

	class MessageParser final : public DiagnosticList<MessageParser>, public Lya::lib::Parser<MessageParser, Token, TokenScanner> {
	public:
		const char* language;
		Messages parse(const u32string& text, const char *language);
		Messages parse(const string& text, const char *language);
		Messages parse_message();
		std::string cldr_path();
		void parse_plural_and_ordinal_category_message_list(shared_ptr<PluralMessage> plural_message);
		bool plural_category_is_supported(const PluralCategory plural_category);
	};
}

#endif //LYA_MESSAGE_PARSER_H
