//
// Created by Tingan Ho on 2017-12-30.
//

#ifndef LYA_SYNTAXES_H
#define LYA_SYNTAXES_H

#include <memory>
#include <vector>
#include <string>

namespace Lya::core::parsers::ldml {

	enum class LdmlToken {
		Unknown,
		EndOfText,

		// Logic,
		LogicalAnd,
		LogicalOr,
		Equality,
		NotEqual,
		Remainder,

		IntegerLiteral,
		DoubleLiteral,

		Range,

		// Samples
		AtInteger,
		AtDecimal,

		// Punctuations
		Comma,
		HorizontalEllipsis,
		Percent,
		Tilde,

		// Operands
		AbsoluteValueTransform, // n
		IntegerValueTransform, // i
		NumberOfVisibleFractionDigits_WithTrailingZeroTransform, // v
		NumberOfVisibleFractionDigits_WithoutTrailingZeroTransform, // w
		VisibleFractionDigits_WithTrailingZeroTransform, // f
		VisibleFractionDigits_WithoutTrailingZeroTransform, // t
	};

	class LdmlNodeVisitor;

	struct Node {
		std::weak_ptr<Node> parent;
		std::vector<std::unique_ptr<Node>> children;
		virtual void accept(LdmlNodeVisitor*) = 0;
	};

	struct Expression : Node {
		void accept(LdmlNodeVisitor*);
	};

	struct TokenNode : Node {
		LdmlToken token;

		TokenNode(LdmlToken _token):
			token(_token) { }

		void accept(LdmlNodeVisitor*);
	};

	struct IntegerLiteral : Expression {
		int value;

		IntegerLiteral(int _value):
			value(_value) { }

		void accept(LdmlNodeVisitor*);
	};

	struct FloatLiteral : Expression {
		double value;

		FloatLiteral(double _value):
			value(_value) { }

		void accept(LdmlNodeVisitor*);
	};

	enum class ValueTransformType {
		IntegerDigitsValue,
		AbsoluteValue,
		NumberOfVisibleFractionDigits_WithTrailingZeros,
		NumberOfVisibleFractionDigits_WithoutTrailingZeros,
		VisibleFractionalDigits_WithTrailingZeros,
		VisibleFractionalDigits_WithoutTrailingZeros,
	};

	struct ValueTransform : Expression {
		ValueTransformType type;

		ValueTransform(ValueTransformType type):
			type(type)
		{ }

		void accept(LdmlNodeVisitor*);
	};

	struct BinaryExpression : Expression {
		std::unique_ptr<Expression> left_operand;
		std::unique_ptr<TokenNode> _operator;
		std::unique_ptr<Expression> right_operand;

		BinaryExpression(
			std::unique_ptr<Expression>& _left,
			std::unique_ptr<TokenNode>& __operator,
			std::unique_ptr<Expression>& _right):

			left_operand(std::move(_left)),
			_operator(std::move(__operator)),
			right_operand(std::move(_right))
		{ }

		void accept(LdmlNodeVisitor*);
	};

	class LdmlNodeVisitor {
	public:
		virtual void visit(const Expression*) = 0;
		virtual void visit(const TokenNode*) = 0;
		virtual void visit(const IntegerLiteral*) = 0;
		virtual void visit(const FloatLiteral*) = 0;
		virtual void visit(const ValueTransform*) = 0;
		virtual void visit(const BinaryExpression*) = 0;
	};
}
#endif //LYA_SYNTAXES_H
