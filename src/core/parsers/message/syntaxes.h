
#ifndef LYA_MESSAGE_SYNTAXES_H
#define LYA_MESSAGE_SYNTAXES_H

#include <vector>
#include <memory>
#include <string>
#include <map>

using namespace std;

namespace Lya::core::parsers::message {

	enum class PluralCategory {
		None,
		Zero,
		One,
		Two,
		Few,
		Many,
		Other,
		Specified,
	};

	enum class OrdinalCategory {
		Zero,
		One,
		Two,
		Few,
		Many,
		Other,
		Specified,
	};

	class Visitor;

	class Message {
	public:
		virtual void accept(class Visitor*) = 0;
	};

	typedef vector<shared_ptr<Message>> Messages;

	class TextMessage : public Message {
	public:
		u32string text;
		TextMessage(u32string _text);
		void accept(Visitor*);
	};

	class InterpolationMessage : public Message {
	public:
		string variable;
		void accept(Visitor*);
	};

	class PluralMessage : public Message {
	public:
		map<PluralCategory, vector<shared_ptr<Message>>> plural_category_messages;
		map<int, vector<shared_ptr<Message>>> value_messages;

		PluralMessage():
			plural_category_messages({}),
			value_messages({})
		{ }

		void accept(Visitor*);
	};


	class Visitor
	{
	public:
		virtual void visit(TextMessage*) = 0;
		virtual void visit(InterpolationMessage*) = 0;
		virtual void visit(PluralMessage*) = 0;
	};
}
#endif // LYA_MESSAGE_SYNTAXES_H