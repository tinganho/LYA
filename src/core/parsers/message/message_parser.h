//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_PARSER_H
#define LYA_MESSAGE_PARSER_H

#include <string>
#include "token_scanner.h"
#include "parser.h"
#include "syntaxes.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {
	class MessageParser : public Diagnostic<MessageParser>, public Lya::lib::Parser<MessageParser, Token> {
	public:
		MessageParser(const char* _language);
		unique_ptr<TokenScanner> scanner;
		const char* language;
		vector<shared_ptr<Message>> messages;
		vector<shared_ptr<Message>> parse(const u32string& text, const char *language);
		vector<shared_ptr<Message>> parse(const string& text, const char *language);
		vector<shared_ptr<Message>> parse_message();
		void parse_plural_category_messages(shared_ptr<PluralMessage> plural_message);
		bool plural_category_is_supported(const PluralCategory plural_category);
	};
}

#endif //LYA_MESSAGE_PARSER_H
