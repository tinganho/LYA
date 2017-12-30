//
// Created by Tingan Ho on 2017-11-12.
//

#ifndef LYA_LDML_H
#define LYA_LDML_H

#include "parser.h"
#include "diagnostic.h"
#include "syntaxes.h"

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {

	class LdmlParser final : public DiagnosticList<LdmlParser>, public Parser<LdmlParser, Token, TokenScanner> {
	public:
		Expression parse(const u32string& text);
	private:
		bool next_token_is_binary_operator();
		bool get_binary_operator_precedence(Token token);
	};
}

#endif //LYA_LDML_H
