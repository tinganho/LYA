//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_PARSER_H
#define LYA_MESSAGE_PARSER_H

#include <glibmm/ustring.h>
#include <memory>
#include "core/parsers/ldml/ldml_parser.h"
#include "core/parsers/ldml/syntaxes.h"
#include "syntaxes.h"
#include <string>
#include <set>
#include "token_scanner.h"
#include "parser.h"

using namespace Lya::lib;

namespace Lya::core::parsers::message {
    struct MessageDependencies {
        std::set<ldml::ValueTransformType> value_transform_types;
    };

	class MessageParser final : public DiagnosticList<MessageParser>, public Lya::lib::Parser<MessageParser, MessageToken, TokenScanner> {
	public:
		MessageParser(const std::string& language);
		std::unique_ptr<Messages> parse(const Glib::ustring& text);
		void read_plural_info();
		std::unique_ptr<std::map<PluralForm, std::unique_ptr<ldml::Expression>>> plural_forms;
		std::unique_ptr<std::vector<types::PluralForm>> supported_plural_forms_excluding_other;
        MessageDependencies get_dependencies();
	private:
        ldml::LdmlParser ldml_parser;
		std::string language;
		bool has_read_plural_info;
        std::unique_ptr<Messages> parse_message(const Glib::ustring& message);
		void parse_plural_and_ordinal_category_message_list(const std::unique_ptr<PluralMessage>& plural_message);
		bool plural_category_is_supported(const PluralForm plural_form);
        std::unique_ptr<Messages> parse_primary();
	};
}

#endif //LYA_MESSAGE_PARSER_H
