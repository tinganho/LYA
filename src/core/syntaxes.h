
namespace Lya::core::syntaxes {

	enum class PluralCategory {
		Zero,
		One,
		Two,
		Few,
		Many,
		Other,
		Specified,
	};

	struct TextSpan {
		unsigned int start;
		unsigned int end;
	};

	typedef struct Fragment Fragment;
	struct Fragment : TextSpan {
		shared_ptr<Node> child;
	};

	struct Message : TextSpan {
		vector<shared_ptr<Fragment>> fragments;
	};

	struct PluralFragment : Fragment {
		map<PluralCategory, Message> plural_category_messages;
		map<int, Message> value_messages;
	};

	struct Interpolation : Fragment {

	};

	struct TextFragment : Fragment {
		u32string text;
	};
}