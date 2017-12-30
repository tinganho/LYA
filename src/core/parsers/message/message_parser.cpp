
#include "core/diagnostics/diagnostics.h"
#include "configurations.h"
#include "message_parser.h"

using namespace Lya::core::diagnostics;

namespace Lya::core::parsers::message {

	Messages MessageParser::parse(const u32string& text, const char* _language)
	{
		language = _language;
		read_file(resolve_paths(LYA_CLDR_DIR, "common/supplemental/plurals.xml"));
		read_file(resolve_paths(LYA_CLDR_DIR, "common/supplemental/ordinals.xml"));
		scanner = make_unique<TokenScanner>(text);
		return parse_message();
	}

	Messages MessageParser::parse(const string& text, const char* language)
	{
		return parse(to_utf32_string(text), language);
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
		Token t = next_token();
		while (t != Token::CloseBrace || t != Token::EndOfFile) {
			PluralCategory category = PluralCategory::None;
			switch (t) {
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
			t = next_token();
		}
	}

	bool MessageParser::plural_category_is_supported(PluralCategory plural_category) {
		return true;
	}
}