//
// Created by Tingan Ho on 2017-12-30.
//

#ifndef LYA_SYNTAXES_H
#define LYA_SYNTAXES_H

#include "token_scanner.h"
#include <memory>

namespace Lya::core::parsers::ldml {

	enum class BinaryOperator {
		Equals,
		Modulo,
		And,
		Or,
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
		Token token;

		TokenNode(Token _token):
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
		IntegerValue,
		AbsoluteValue,
		NumberOfVisibleFractionDigits_WithTrailingZero,
		NumberOfVisibleFractionDigits_WithoutTrailingZero,
	};

	struct ValueTransform : Expression {
		ValueTransformType type;

		ValueTransform(ValueTransformType _type):
			type(_type)
		{ }

		void accept(LdmlNodeVisitor*);
	};

	struct BinaryExpression : Expression {
		std::unique_ptr<Expression> left;
		std::unique_ptr<TokenNode> _operator;
		std::unique_ptr<Expression> right;

		BinaryExpression(
			std::unique_ptr<Expression>& _left,
			std::unique_ptr<TokenNode>& __operator,
			std::unique_ptr<Expression>& _right):

			left(std::move(_left)),
			_operator(std::move(__operator)),
			right(std::move(_right))
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
