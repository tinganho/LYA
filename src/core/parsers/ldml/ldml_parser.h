//
// Created by Tingan Ho on 2017-11-12.
//

#ifndef LYA_LDML_H
#define LYA_LDML_H

#include <core/parsers/ldml/syntaxes.h>
#include <glibmm/ustring.h>
#include "parser.h"
#include "diagnostic.h"
#include "syntaxes.h"
#include "token_scanner.h"
#include <memory>
#include <set>

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {
	class LdmlParser final : public DiagnosticList<LdmlParser>, public Parser<LdmlParser, LdmlToken, TokenScanner> {
	public:
		std::unique_ptr<Expression> parse(const Glib::ustring& text);
        std::set<ValueTransformType> get_value_transform_types();
	private:
        std::set<ValueTransformType> value_transform_types;
		std::unique_ptr<Expression> parse_right_operand(int left_precedence, std::unique_ptr<Expression> left_operand);
		std::unique_ptr<Expression> parse_primary_expression();
		std::unique_ptr<Expression> parse_binary_expression_or_higher(int left_precedence);
		int get_token_precedence(LdmlToken token);
	};
}

#endif //LYA_LDML_H
