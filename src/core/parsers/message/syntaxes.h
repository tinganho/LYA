
#ifndef LYA_PARSERS_MESSAGE_H
#define LYA_PARSERS_MESSAGE_H
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

	struct Message { };

	struct TextMessage : Message {
		u32string text;
		TextMessage(u32string _text): text(_text) { }
	};

	struct InterpolationMessage : Message {
		string variable;
	};

	struct PluralMessage : Message {
		map<PluralCategory, vector<shared_ptr<Message>>> plural_category_messages;
		map<int, vector<shared_ptr<Message>>> value_messages;
		PluralMessage():
			plural_category_messages({}),
			value_messages({})
		{ }
	};
}
#endif //LYA_PARSERS_MESSAGE_H