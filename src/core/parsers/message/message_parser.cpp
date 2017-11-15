
#include "message_parser.h"
#include "diagnostics/diagnostics.h"
#include "syntaxes.h"

using namespace Lya::core::diagnostics;

namespace Lya::core::parsers::message {
	MessageParser::MessageParser(const char* _language):
		message() {
	}

	Message MessageParser::parse(const u32string& text, const char* _language) {
		language = _language;
		scanner = make_unique<TokenScanner>(text);

		return parse_message();
	}

	Message MessageParser::parse(const string& text, const char* language) {
		return parse(to_u32_string(text), language);
	}

	Message MessageParser::parse_message() {
		Message message;
		while (true) {
			Token t = next_token();
			if (t == Token::EndOfFile) {
				goto end;
			}
			switch (t) {
				case Token::OpenBrace: {
					if (next_token_is(Token::Identifier)) {
						const u32string& identifier = get_value();
						if (next_token_is(Token::Comma)) {
							Token t = next_token();
							switch (t) {
								case Token::PluralKeyword: {
									shared_ptr<PluralFragment> plural_fragment = make_shared<PluralFragment>();
									parse_plural_category_messages(plural_fragment);
									message.fragments.push_back(plural_fragment);
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
		return message;
	}

	void MessageParser::parse_plural_category_messages(const shared_ptr<PluralFragment> plural_fragment) {
		const Token t = next_token();
		switch (t) {
			case Token::ZeroKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Zero);
					plural_fragment->plural_category_messages[PluralCategory::Zero] = parse_message();
				}
				break;
			case Token::OneKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::One);
					plural_fragment->plural_category_messages[PluralCategory::One] = parse_message();
				}
				break;
			case Token::TwoKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Two);
					plural_fragment->plural_category_messages[PluralCategory::Two] = parse_message();
				}
				break;
			case Token::FewKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Few);
					plural_fragment->plural_category_messages[PluralCategory::Few] = parse_message();
				}
				break;
			case Token::ManyKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_category_is_supported(PluralCategory::Many);
					plural_fragment->plural_category_messages[PluralCategory::Many] = parse_message();
				}
				break;
			case Token::OtherKeyword:
				if (next_token_is(Token::OpenBrace)) {
					plural_fragment->plural_category_messages[PluralCategory::Other] = parse_message();
				}
				break;
			case Token::Equals:
				if (next_token_is(Token::Number)) {
					if (next_token_is(Token::OpenBrace)) {
						int i = stoi(get_utf8_value());
						auto messages = plural_fragment->value_messages;
						auto it = messages.find(i);
						if (it != messages.end()) {
							add_diagnostic(D::Duplicate_integer_message_0, to_string(i));
							break;
						}
						messages[i] = parse_message();
					}
					break;
				}
			default:
				throw logic_error("Unknown plural category.");
		}
	}

	bool MessageParser::plural_category_is_supported(PluralCategory plural_category) {

	}
}