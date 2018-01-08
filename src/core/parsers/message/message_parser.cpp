
#include "core/diagnostics/diagnostics.h"
#include "core/parsers/ldml/ldml_parser.h"
#include "message_parser.h"
#include "core/parsers/external_data.h"
#include <glibmm/ustring.h>

using namespace Lya::core::diagnostics;
using namespace Lya::core::parsers::ldml;

namespace Lya::core::parsers::message {

	MessageParser::MessageParser(const std::string& language):
		language(std::move(language)),
		has_read_plural_info(false)
	{ }

	std::unique_ptr<Messages> MessageParser::parse(const Glib::ustring& text)
	{
		return parse_message(text);
	}

    std::unique_ptr<Messages> MessageParser::parse_primary()
    {
        std::unique_ptr<Messages> messages = std::make_unique<Messages>();
        while (true) {
            MessageToken t = next_token();
            switch (t) {
                case MessageToken::EndOfFile:
                case MessageToken::CloseBrace:
                    return messages;

                case MessageToken::Text: {
                    std::unique_ptr<TextMessage> text_message = std::make_unique<TextMessage>(get_token_value());
                    messages->push_back(std::move(text_message));
                    break;
                }

                case MessageToken::OpenBrace: {
                    if (next_token_is(MessageToken::Identifier)) {
                        const std::string& variable = get_token_value();
                        if (next_token_is(MessageToken::Comma)) {
                            MessageToken t = next_token();
                            switch (t) {
                                case MessageToken::PluralKeyword: {
                                    if (!has_read_plural_info) {
                                        read_plural_info();
                                        has_read_plural_info = true;
                                    }
                                    std::unique_ptr<PluralMessage> plural_message = std::make_unique<PluralMessage>();
                                    if (next_token_is(MessageToken::Comma)) {
                                        parse_plural_and_ordinal_category_message_list(plural_message);
                                    }
                                    plural_message->variable = variable;
                                    messages->push_back(std::move(plural_message));
                                    break;
                                }
                                case MessageToken::ContextKeyword:
                                case MessageToken::NumberKeyword:
                                case MessageToken::DateKeyword:
                                case MessageToken::ListKeyword:
                                case MessageToken::OrdinalKeyword:
                                case MessageToken::AttributeKeyword:
                                default:;
                            }
                        }
                    }
                    break;
                }
                default:;
            }
        }

        return messages;
    }

    std::unique_ptr<Messages> MessageParser::parse_message(const Glib::ustring& text)
	{
        scanner = std::make_unique<TokenScanner>(text);
        return parse_primary();
	}

	/// This function expects a plural category message list of the form
	///
	///   "one {...} other {...}"
	///
	/// It will terminate on an ending "}" or when it reaches end of file.
	///
	/// \param plural_message
	///
	void MessageParser::parse_plural_and_ordinal_category_message_list(const std::unique_ptr<PluralMessage>& plural_message) {
		MessageToken token = next_token();
		while (token != MessageToken::CloseBrace || token != MessageToken::EndOfFile) {
			PluralForm plural_form = PluralForm::None;
			switch (token) {
				case MessageToken::ZeroKeyword:
					plural_form = PluralForm::Zero;
					break;
				case MessageToken::OneKeyword:
					plural_form = PluralForm::One;
					break;
				case MessageToken::TwoKeyword:
					plural_form = PluralForm::Two;
					break;
				case MessageToken::FewKeyword:
					plural_form = PluralForm::Few;
					break;
				case MessageToken::ManyKeyword:
					plural_form = PluralForm::Many;
					break;
				case MessageToken::OtherKeyword:
					plural_form = PluralForm::Other;
					break;
				case MessageToken::Equals:
					if (next_token_is(MessageToken::Number)) {
						if (next_token_is(MessageToken::OpenBrace)) {
							int i = std::stoi(get_token_value());
							std::map<int, std::unique_ptr<Messages>>& messages = plural_message->value_messages;
							auto it = messages.find(i);
							if (it != messages.end()) {
								add_diagnostic(D::Duplicate_value_message_0, to_string(i));
								break;
							}
							messages[i] = parse_primary();
						}
						break;
					}
				default:
					add_diagnostic(D::Unknown_plural_category);
					return;
			}
			if (plural_form != PluralForm::None) {
				if (next_token_is(MessageToken::OpenBrace)) {
					if (plural_category_is_supported(plural_form)) {
                        plural_message->plural_form_messages[plural_form] = parse_primary();
					}
                    else {
                        add_diagnostic(D::Plural_form_0_is_not_supported_in_language_1, plural_form_to_string.at(plural_form), language);
                    }
					if (plural_form == PluralForm::Other) {
                        next_token_is(MessageToken::CloseBrace);
						return;
					}
				}
			}
			token = next_token();
		}
	}

	void MessageParser::read_plural_info()
	{
		std::unique_ptr<PluralInfo> plural_info = Lya::core::parsers::read_plural_info(language);
		supported_plural_forms_excluding_other = std::move(plural_info->supported_plural_forms_excluding_other);
		plural_forms = std::make_unique<std::map<PluralForm, std::unique_ptr<ldml::Expression> > >();
		for (const PluralForm& plural_category : *supported_plural_forms_excluding_other) {
			std::unique_ptr<Expression> expression = ldml_parser.parse((*plural_info->plural_rules)[(PluralForm)plural_category]);
			(*plural_forms)[plural_category] = std::move(expression);
		}
	}

    MessageDependencies MessageParser::get_dependencies()
    {
        return MessageDependencies {
            ldml_parser.get_value_transform_types(),
        };
    }

	bool MessageParser::plural_category_is_supported(PluralForm plural_form) {
		if (plural_forms->empty()) {
			throw logic_error("You have not read plural info yet.");
		}
        if (plural_form == PluralForm::Other) {
            return true;
        }
		return plural_forms->find(plural_form) != plural_forms->end();
	}
}