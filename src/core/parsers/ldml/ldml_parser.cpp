
#include "token_scanner.h"
#include "ldml_parser.h"
#include <string>
#include <memory>

namespace Lya::core::parsers::ldml {

	Expression LdmlParser::parse(const u32string& text) {
		Expression expression;
		scanner = make_unique<TokenScanner>(text);

		while (true) {
			Token t = next_token();
			switch (t) {
				case Token::IntegerValueTransform:
					if (next_token_is(Token::Equal)) {
						if (next_token_is(Token::Integer)) {
							std::shared_ptr<BinaryExpression> binary_expression = std::make_shared(new BinaryExpression({
								ValueTransform { ValueTransformType::IntegerValue },
								BinaryOperator::Equals,
								Integer { std::stoi(get_utf8_value()) },
							}));
							if (next_token_is_binary_operator()) {
								Token token = next_token();
								int precedence = get_binary_operator_precedence(token);
								if (precedence) {

								}
							}
							expression.children.push_back(binary_expression);
						}
					}
					else {
						throw logic_error("Was expecting an equals sign after integer value transform.");
					}
					break;
				case Token::AbsoluteValueTransform:
					break;
				case Token::EndOfText;
					goto end;

			}
		}

	end:
		return expression;
	}


	bool LdmlParser::next_token_is_binary_operator() {
		Token token = peek_next_token();
		switch (token) {
			case Token::And:
			case Token::Or:
			case Token::Equal:
			case Token::Modulo:
				return true;
		}
		return false;
	}

	bool LdmlParser::get_binary_operator_precedence(Token token) {
		switch (token) {
			case Token::And:
				return 1;
			case Token::Or:
				return 2;
			case Token::Equal:
			case Token::Modulo:
				return 3;
		}
	}
}