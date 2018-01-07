//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_PARSER_H
#define LYA_MESSAGE_PARSER_H

#include "core/parsers/ldml/syntaxes.h"
#include "syntaxes.h"
#include <string>
#include "token_scanner.h"
#include "parser.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {

	class MessageParser final : public DiagnosticList<MessageParser>, public Lya::lib::Parser<MessageParser, MessageToken, TokenScanner> {
	public:
		MessageParser(const std::string& language);
		Messages parse(const std::u32string& text);
		Messages parse(const std::string& text);
        Messages parse_messages(const std::vector<std::string>& messages);
		void read_plural_info();
		std::unique_ptr<std::map<PluralForm, std::unique_ptr<ldml::Expression>>> plural_forms;
		std::unique_ptr<std::vector<types::PluralForm>> supported_plural_forms_excluding_other;
	private:
		std::string language;
		bool has_read_plural_info;
		Messages parse_message();
		void parse_plural_and_ordinal_category_message_list(const std::unique_ptr<PluralMessage>& plural_message);
		bool plural_category_is_supported(const PluralForm plural_form);
	};
}

#endif //LYA_MESSAGE_PARSER_H
