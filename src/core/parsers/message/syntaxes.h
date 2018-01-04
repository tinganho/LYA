
#ifndef LYA_MESSAGE_SYNTAXES_H
#define LYA_MESSAGE_SYNTAXES_H

#include <lib/types.h>
#include <vector>
#include <memory>
#include <string>
#include <map>

using namespace Lya::lib::types;

namespace Lya::core::parsers::message {

	class MessageNodeVisitor;

	class Message {
	public:
		virtual void accept(class MessageNodeVisitor*) const = 0;
	};

	typedef std::vector<std::shared_ptr<Message>> Messages;

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

	class PluralMessage : public Message {
	public:
		std::map<PluralCategory, std::vector<std::shared_ptr<Message>>> plural_category_messages;
		std::map<int, std::vector<std::shared_ptr<Message>>> value_messages;

		PluralMessage():
			plural_category_messages({}),
			value_messages({})
		{ }

		void accept(MessageNodeVisitor*) const;
	};

	class OrdinalMessage: public Message {
	public:
		std::map<PluralCategory, std::vector<std::shared_ptr<Message>>> plural_category_messages;
		std::map<int, std::vector<std::shared_ptr<Message>>> value_messages;

		OrdinalMessage():
			plural_category_messages({}),
			value_messages({})
		{ }

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