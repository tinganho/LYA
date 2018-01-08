//
// Created by Tingan Ho on 2017-11-08.
//

#ifndef LYA_PARSER_H
#define LYA_PARSER_H

#include "types.h"
#include "scanner.h"

using namespace Lya::lib::types;

namespace Lya::lib {
	template<typename TParser, typename TToken, typename TTokenScanner>
	class Parser {
	public:
		SpanLocation get_token_location()
		{
			return static_cast<TParser*>(this)->scanner->get_token_location();
		}

		bool next_token_is(TToken expected_token)
		{
            static_cast<TParser*>(this)->scanner->save();
			TToken current_token = next_token();
			if (current_token == expected_token) {
				return true;
			}
            static_cast<TParser*>(this)->scanner->revert();
			return false;
		}

		bool next_token_is_identifier(const Glib::ustring& expected_identifier)
		{
			TToken current_token = next_token();
			if (!next_token_is(TToken::Identifier)) {
				return false;
			}
			const Glib::ustring& current_identifier = static_cast<TParser*>(this)->scanner->get_value();
			if (current_identifier == expected_identifier) {
				return true;
			}
			static_cast<TParser*>(this)->add_diagnostic(
					DiagnosticTemplate { "Expected identifier '{0}' but got '{1}'." },
					expected_identifier,
                    current_identifier);
			return false;
		}

		TToken next_token()
		{
			return static_cast<TParser*>(this)->scanner->next_token();
		}

		TToken peek_next_token()
		{
			return static_cast<TParser*>(this)->scanner->peek_next_token();
		}

        Glib::ustring get_token_value()
		{
			return static_cast<TParser*>(this)->scanner->get_token_value();
		}

	protected:
		std::unique_ptr<TTokenScanner> scanner;
	};
}


#endif //LYA_PARSER_H
