
#include "token_scanner.h"
#include "ldml_parser.h"
#include <string>
#include <memory>

namespace Lya::core::parsers::ldml {

	std::unique_ptr<Expression> LdmlParser::parse(const std::string& text)
	{
		return parse(scanner->to_utf32_string(text));
	}

	std::unique_ptr<Expression> LdmlParser::parse(const std::u32string& text)
	{
		std::unique_ptr<Expression> expression = std::make_unique<Expression>();
		scanner = std::make_unique<TokenScanner>(text);
		return parse_binary_expression_or_higher(/* left_precedence */ 0);

	}

	std::unique_ptr<Expression> LdmlParser::parse_binary_expression_or_higher(int left_precedence)
	{
		std::unique_ptr<Expression> left_operand = parse_primary_expression();
		return parse_right_operand(left_precedence, std::move(left_operand));
	}

	std::unique_ptr<Expression> LdmlParser::parse_primary_expression()
	{
		Token token = next_token();
		switch (token) {
			case Token::IntegerValueTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::IntegerValue);
			case Token::AbsoluteValueTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::AbsoluteValue);
			case Token::NumberOfVisibleFractionDigits_WithTrailingZeroTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::NumberOfVisibleFractionDigits_WithTrailingZero);
			case Token::NumberOfVisibleFractionDigits_WithoutTrailingZeroTransform:
				return std::make_unique<ValueTransform>(ValueTransformType::NumberOfVisibleFractionDigits_WithoutTrailingZero);
			case Token::DoubleLiteral:
				return std::make_unique<FloatLiteral>(std::stod(get_utf8_value()));
			case Token::IntegerLiteral:
				return std::make_unique<IntegerLiteral>(std::stoi(get_utf8_value()));
			default:;
		}
		throw logic_error("Could not understand primary expression.");
	}

	std::unique_ptr<Expression> LdmlParser::parse_right_operand(
			int left_precedence,
			std::unique_ptr<Expression> left_operand)
	{
		while (true) {
			scanner->save();
			Token token = next_token();
			int right_precedence = get_token_precedence(token);
			if (left_precedence >= right_precedence) {
				scanner->revert();
				break;
			}
			std::unique_ptr<TokenNode> _operator = std::make_unique<TokenNode>(token);
			std::unique_ptr<Expression> right_operand = parse_binary_expression_or_higher(right_precedence);
			left_operand = std::make_unique<BinaryExpression>(
				left_operand,
				_operator,
				right_operand
			);
		}
		return left_operand;
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
			default:;
		}
		return -1;
	}
}