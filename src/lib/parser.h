//
// Created by Tingan Ho on 2017-11-08.
//

#ifndef LYA_PARSER_H
#define LYA_PARSER_H

#include "types.h"
#include "scanner.h"

using namespace Lya::lib::types;

namespace Lya::lib {
	template<typename D, typename T>
	class Parser {
	public:
		SpanLocation get_token_location() {
			return static_cast<D*>(this)->scanner->get_token_location();
		}

		bool next_token_is(T expected_token) {
			T current_token = next_token();
			if (current_token == expected_token) {
				return true;
			}
			static_cast<D*>(this)->add_diagnostic(
					DiagnosticTemplate { "Expected token '{0}' but got '{1}'." },
					static_cast<D*>(this)->scanner->to_utf8_string((*static_cast<D*>(this)->scanner->token_enum_to_string)[expected_token]),
					static_cast<D*>(this)->scanner->to_utf8_string((*static_cast<D*>(this)->scanner->token_enum_to_string)[current_token]));
			return false;
		}

		bool next_token_is_identifier(const u32string &expected_identifier) {
			T current_token = next_token();
			if (!next_token_is(T::Identifier)) {
				return false;
			}
			const u32string& current_identifier = static_cast<D*>(this)->scanner->get_value();
			if (current_identifier == expected_identifier) {
				return true;
			}
			static_cast<D*>(this)->add_diagnostic(
					DiagnosticTemplate { "Expected identifier '{0}' but got '{1}'." },
					static_cast<D*>(this)->scanner->to_utf8_string(expected_identifier),
					static_cast<D*>(this)->scanner->to_utf8_string(current_identifier));
			return false;
		}

		T next_token() {
			return static_cast<D*>(this)->scanner->next_token();
		}

		u32string get_value() {
			return static_cast<D*>(this)->scanner->get_value();
		}

		string get_utf8_value() {
			return static_cast<D*>(this)->scanner->to_utf8_string(get_value());
		}

		u32string to_u32_string(const string &text) {
			return static_cast<D*>(this)->scanner->to_u32_string(text);
		}
	};
}


#endif //LYA_PARSER_H
