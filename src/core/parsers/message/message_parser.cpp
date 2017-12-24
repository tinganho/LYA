
#include "message_parser.h"
#include "diagnostics/diagnostics.h"
#include "syntaxes.h"

using namespace Lya::core::diagnostics;

namespace Lya::core::parsers::message {

	MessageParser::MessageParser(const char* _language):
		messages() { }

	vector<shared_ptr<Message>> MessageParser::parse(const u32string& text, const char* _language)
	{
		language = _language;
		scanner = make_unique<TokenScanner>(text);

		return parse_message();
	}

	vector<shared_ptr<Message>> MessageParser::parse(const string& text, const char* language)
	{
		return parse(to_u32_string(text), language);
	}

	vector<shared_ptr<Message>> MessageParser::parse_message()
	{
		vector<shared_ptr<Message>> m;
		while (true) {
			Token t = next_token();
			if (t == Token::EndOfFile) {
				goto end;
			}
			switch (t) {
				case Token::Text: {
					shared_ptr<TextMessage> text_message = make_shared<TextMessage>(TextMessage(get_value()));
					m.push_back(text_message);
					continue;
				}
				case Token::OpenBrace: {
					if (next_token_is(Token::Identifier)) {
						const u32string &identifier = get_value();
						if (next_token_is(Token::Comma)) {
							Token t = next_token();
							switch (t) {
								case Token::PluralKeyword: {
									shared_ptr<PluralMessage> plural_message = make_shared<PluralMessage>(PluralMessage());
									parse_plural_category_messages(plural_message);
									m.push_back(plural_message);
									break;
								}
								case Token::ContextKeyword:
								case Token::NumberKeyword:
								case Token::DateKeyword:
								case Token::ListKeyword:
								case Token::OrdinalKeyword:
								default:;
							}
						}
					}
				}
				default:;
			}
		}

		end:
		return messages;
	}

	void MessageParser::parse_plural_category_messages(shared_ptr<PluralMessage> plural_message) {
		const Token t = next_token();
		switch (t) {
			case Token::ZeroKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Zero);
					plural_message->plural_category_messages[PluralCategory::Zero] = parse_message();
				}
				break;
			case Token::OneKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::One);
					plural_message->plural_category_messages[PluralCategory::One] = parse_message();
				}
				break;
			case Token::TwoKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Two);
					plural_message->plural_category_messages[PluralCategory::Two] = parse_message();
				}
				break;
			case Token::FewKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Few);
					plural_message->plural_category_messages[PluralCategory::Few] = parse_message();
				}
				break;
			case Token::ManyKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Many);
					plural_message->plural_category_messages[PluralCategory::Many] = parse_message();
				}
				break;
			case Token::OtherKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_message->plural_category_messages[PluralCategory::Other] = parse_message();
				}
				break;
			case Token::Equals:
				if (next_token_is(Token::Number)) {
					if (next_token_is(Token::OpenBrace)) {
						int i = stoi(get_utf8_value());
						auto messages = plural_message->value_messages;
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
		}
	}

	bool MessageParser::plural_category_is_supported(PluralCategory plural_category) {

	}
}