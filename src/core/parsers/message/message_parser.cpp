
#include "core/diagnostics/diagnostics.h"
#include "core/parsers/ldml/ldml_parser.h"
#include "message_parser.h"
#include "lib/external_data.h"

using namespace Lya::core::diagnostics;
using namespace Lya::core::parsers::ldml;

namespace Lya::core::parsers::message {

	MessageParser::MessageParser(const std::string& language):
		language(std::move(language)),
		has_read_plural_info(false)
	{ }

	Messages MessageParser::parse(const std::u32string& text)
	{
		scanner = std::make_unique<TokenScanner>(text);
		return parse_message();
	}

	Messages MessageParser::parse(const std::string& text)
	{
		return parse(to_utf32_string(text));
	}

	Messages MessageParser::parse_message()
	{
		Messages messages;
		while (true) {
			Token t = next_token();
			switch (t) {
				case Token::EndOfFile:
				case Token::CloseBrace:
					return messages;

				case Token::Text: {
					shared_ptr<TextMessage> text_message = make_shared<TextMessage>(TextMessage(get_utf8_value()));
					messages.push_back(text_message);
					break;
				}

				case Token::OpenBrace: {
					if (next_token_is(Token::Identifier)) {
						const u32string identifier = get_value();
						if (next_token_is(Token::Comma)) {
							Token t = next_token();
							switch (t) {
								case Token::PluralKeyword: {
									if (plural_rules->size() == 0) {
										read_plural_info();
									}
									shared_ptr<PluralMessage> plural_message = make_shared<PluralMessage>(PluralMessage());
									if (next_token_is(Token::Comma)) {
										parse_plural_and_ordinal_category_message_list(plural_message);
									}
									messages.push_back(plural_message);
									break;
								}
								case Token::ContextKeyword:
								case Token::NumberKeyword:
								case Token::DateKeyword:
								case Token::ListKeyword:
								case Token::OrdinalKeyword:
								case Token::AttributeKeyword:
								default:;
							}
						}
					}
					break;
				}
				default:;
			}
		}
	}

	/// This function expects a plural category message list of the form
	///
	///   "one {...} other {...}"
	///
	/// It will terminate on an ending "}" or when it reaches end of file.
	///
	/// \param plural_message
	///
	void MessageParser::parse_plural_and_ordinal_category_message_list(shared_ptr<PluralMessage> plural_message) {
		Token token = next_token();
		while (token != Token::CloseBrace || token != Token::EndOfFile) {
			PluralCategory category = PluralCategory::None;
			switch (token) {
				case Token::ZeroKeyword:
					category = PluralCategory::Zero;
					break;
				case Token::OneKeyword:
					category = PluralCategory::One;
					break;
				case Token::TwoKeyword:
					category = PluralCategory::Two;
					break;
				case Token::FewKeyword:
					category = PluralCategory::Few;
					break;
				case Token::ManyKeyword:
					category = PluralCategory::Many;
					break;
				case Token::OtherKeyword:
					category = PluralCategory::Other;
					break;
				case Token::Equals:
					if (next_token_is(Token::Number)) {
						if (next_token_is(Token::OpenBrace)) {
							int i = std::stoi(get_utf8_value());
							std::map<int, std::vector<std::shared_ptr<Message>>> messages = plural_message->value_messages;
							auto it = messages.find(i);
							if (it != messages.end()) {
								add_diagnostic(D::Duplicate_value_message_0, to_string(i));
								break;
							}
							messages[i] = parse_message();
						}
						break;
					}
				default:
					add_diagnostic(D::Unknown_plural_category);
					return;
			}
			if (category != PluralCategory::None) {
				if (next_token_is(Token::OpenBrace)) {
					if (plural_category_is_supported(category)) {
						plural_message->plural_category_messages[category] = parse_message();
					}
					if (category == PluralCategory::Other) {
						next_token_is(Token::CloseBrace);
						return;
					}
				}
			}
			token = next_token();
		}
	}

	void MessageParser::read_plural_info()
	{
		if (has_read_plural_info) {
			return;
		}
		std::unique_ptr<PluralInfo> plural_info = lib::read_plural_info(language);
		LdmlParser ldml_parser;
		supported_plural_categories = std::move(plural_info->supported_plural_categories);
		for (const PluralCategory& plural_category : *supported_plural_categories) {
			std::unique_ptr<Expression> expression = ldml_parser.parse((*plural_info->rules)[(PluralCategory)plural_category]);
			(*plural_rules)[plural_category] = std::move(expression);
		}
		has_read_plural_info = true;
	}

	bool MessageParser::plural_category_is_supported(PluralCategory plural_category) {
		if (plural_rules->size() == 0) {
			throw logic_error("You have not read plural info yet.");
		}
		auto it = plural_rules->find(plural_category);
		if (it != plural_rules->end()) {
			return true;
		}
		return false;
	}
}