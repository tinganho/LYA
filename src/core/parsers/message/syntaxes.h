
#ifndef LYA_MESSAGE_SYNTAXES_H
#define LYA_MESSAGE_SYNTAXES_H

#include <lib/types.h>
#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <map>
using namespace Lya::lib::types;

namespace Lya::core::parsers::message {

	enum class MessageToken {
		Identifier,

		// Symbols
		OpenBrace,
		CloseBrace,
		Comma,
		Equals,

		Number,

		// Type keywords
		PluralKeyword,
		OrdinalKeyword,
		ContextKeyword,
		NumberKeyword,
		CurrencyKeyword,
		DateKeyword,
		ListKeyword,
		AttributeKeyword,

		// Plural category keywords
		ZeroKeyword,
		OneKeyword,
		TwoKeyword,
		FewKeyword,
		ManyKeyword,
		OtherKeyword,

		Text,
		EndOfFile,
	};

	const std::map<MessageToken, std::u32string> ldml_token_enum_to_string = {
		{ MessageToken::PluralKeyword, U"plural" },
		{ MessageToken::OrdinalKeyword, U"ordinal" },
		{ MessageToken::ContextKeyword, U"context" },
		{ MessageToken::NumberKeyword, U"number" },
		{ MessageToken::CurrencyKeyword, U"currency" },
		{ MessageToken::DateKeyword, U"date" },
		{ MessageToken::ListKeyword, U"list" },

		{ MessageToken::ZeroKeyword, U"zero" },
		{ MessageToken::OneKeyword, U"one" },
		{ MessageToken::TwoKeyword, U"two" },
		{ MessageToken::FewKeyword, U"few" },
		{ MessageToken::ManyKeyword, U"many" },
		{ MessageToken::OtherKeyword, U"other" },
	};

	class MessageNodeVisitor;

	class Message {
	public:
		virtual void accept(class MessageNodeVisitor*) const = 0;
	};

	typedef std::vector<std::unique_ptr<Message>> Messages;

	class TextMessage : public Message {
	public:
		std::string text;
		TextMessage(std::string _text);
		void accept(MessageNodeVisitor*) const;
	};

	class InterpolationMessage : public Message {
	public:
		std::string variable;
		void accept(MessageNodeVisitor*) const;
	};

	struct PluralMessage : public Message {
		std::string variable;
		std::map<PluralForm, Messages> plural_form_messages;
		std::map<int, Messages> value_messages;
		void accept(MessageNodeVisitor*) const;
	};

	class OrdinalMessage: public Message {
	public:
		std::map<PluralForm, Messages> ordinal_form_messages;

		OrdinalMessage() { }

		void accept(MessageNodeVisitor*) const;
	};


	class MessageNodeVisitor
	{
	public:
		virtual void visit(const TextMessage*) = 0;
		virtual void visit(const InterpolationMessage*) = 0;
		virtual void visit(const PluralMessage*) = 0;
	};


}
#endif // LYA_MESSAGE_SYNTAXES_H