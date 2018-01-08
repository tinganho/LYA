
#include "token_scanner.h"
#include "ldml_parser.h"
#include <string>
#include <memory>

namespace Lya::core::parsers::ldml {
    std::unique_ptr<Expression> LdmlParser::parse(const Glib::ustring& text)
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
		LdmlToken token = next_token();
		switch (token) {
			case LdmlToken::IntegerValueTransform:
                value_transform_types.insert(ValueTransformType::IntegerDigitsValue);
				return std::make_unique<ValueTransform>(ValueTransformType::IntegerDigitsValue);
			case LdmlToken::AbsoluteValueTransform:
                value_transform_types.insert(ValueTransformType::AbsoluteValue);
				return std::make_unique<ValueTransform>(ValueTransformType::AbsoluteValue);
			case LdmlToken::NumberOfVisibleFractionDigits_WithTrailingZeroTransform:
                value_transform_types.insert(ValueTransformType::NumberOfVisibleFractionDigits_WithTrailingZeros);
				return std::make_unique<ValueTransform>(ValueTransformType::NumberOfVisibleFractionDigits_WithTrailingZeros);
			case LdmlToken::NumberOfVisibleFractionDigits_WithoutTrailingZeroTransform:
                value_transform_types.insert(ValueTransformType::NumberOfVisibleFractionDigits_WithoutTrailingZeros);
				return std::make_unique<ValueTransform>(ValueTransformType::NumberOfVisibleFractionDigits_WithoutTrailingZeros);
            case LdmlToken::VisibleFractionDigits_WithTrailingZeroTransform:
                value_transform_types.insert(ValueTransformType::VisibleFractionalDigits_WithTrailingZeros);
                return std::make_unique<ValueTransform>(ValueTransformType::VisibleFractionalDigits_WithTrailingZeros);
            case LdmlToken::VisibleFractionDigits_WithoutTrailingZeroTransform:
                value_transform_types.insert(ValueTransformType::VisibleFractionalDigits_WithoutTrailingZeros);
                return std::make_unique<ValueTransform>(ValueTransformType::VisibleFractionalDigits_WithoutTrailingZeros);
			case LdmlToken::DoubleLiteral:
				return std::make_unique<FloatLiteral>(std::stod(get_token_value()));
			case LdmlToken::IntegerLiteral:
				return std::make_unique<IntegerLiteral>(std::stoi(get_token_value()));
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
			LdmlToken token = next_token();
			int right_precedence = get_token_precedence(token);
			if (left_precedence >= right_precedence) {
				scanner->revert();
				break;
			}
            scanner->dispose_last_save();
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

    std::set<ValueTransformType> LdmlParser::get_value_transform_types()
    {
        return value_transform_types;
    }

	int LdmlParser::get_token_precedence(LdmlToken token)
	{
		switch (token) {
			case LdmlToken::LogicalOr:
				return 1;
			case LdmlToken::LogicalAnd:
				return 2;
			case LdmlToken::Equal:
				return 3;
			case LdmlToken::Remainder:
				return 4;
			default:;
		}
		return -1;
	}
}