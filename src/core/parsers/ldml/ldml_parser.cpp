
#include "token_scanner.h"
#include "ldml_parser.h"
#include <string>
#include <memory>

namespace Lya::core::parsers::ldml {

	std::unique_ptr<Expression> LdmlParser::parse(const u32string& text)
	{
		std::unique_ptr<Expression> expression = std::make_unique<Expression>();
		scanner = make_unique<TokenScanner>(text);
		return parse_binary_expression_or_higher();
	}

	std::unique_ptr<Expression> LdmlParser::parse_binary_expression_or_higher()
	{
		std::unique_ptr<Expression> left_hand_side = parse_primary_expression();
		return parse_right_hand_side(/* left_precedence */ 0, std::move(left_hand_side));
	}

	std::unique_ptr<Expression> LdmlParser::parse_primary_expression()
	{
		Token token = next_token();
		switch (token) {
			case Token::IntegerValueTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::IntegerValue);
			case Token::AbsoluteValueTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::AbsoluteValue);
			case Token::FloatLiteral:
				return std::make_unique<FloatLiteral>(std::stod(get_utf8_value()));
			case Token::IntegerLiteral:
				return std::make_unique<IntegerLiteral>(std::stoi(get_utf8_value()));
		}
		throw logic_error("Could not understand primary expression.");
	}

	std::unique_ptr<Expression> LdmlParser::parse_right_hand_side(
		int left_precedence,
		std::unique_ptr<Expression> left)
	{
		while (true) {
			Token token = next_token();
			int right_precedence = get_token_precedence(token);
			if (left_precedence >= right_precedence) {
				return left;
			}
			std::unique_ptr<TokenNode> _operator = std::make_unique<TokenNode>(token);
			std::unique_ptr<Expression> right = parse_binary_expression_or_higher();
			return std::make_unique<BinaryExpression>(
				left,
				_operator,
				right
			);
		}
	}

	int LdmlParser::get_token_precedence(Token token)
	{
		switch (token) {
			case Token::LogicalOr:
				return 1;
			case Token::LogicalAnd:
				return 2;
			case Token::Equality:
				return 3;
			case Token::Remainder:
				return 4;
		}
		return -1;
	}
}