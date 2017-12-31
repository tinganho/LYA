//
// Created by Tingan Ho on 2017-12-30.
//

#ifndef LYA_SYNTAXES_H
#define LYA_SYNTAXES_H

#include <memory>

namespace Lya::core::parsers::ldml {

	enum class BinaryOperator {
		Equals,
		Modulo,
		And,
		Or,
	};

	struct Node {
		shared_ptr<Node> parent;
		std::vector<std::shared_ptr<Expression>> children;

		Node():
			children({})
		{ }
	};

	struct Expression : Node { };

	struct TokenNode : Node {
		Token token;

		TokenNode(Token _token):
			token(_token) { }
	};

	enum class IntegerRepresentation {
		Regular,
		HexEncoded,
		Binary,
	};

	struct IntegerLiteral : Expression {
		int value;
		IntegerRepresentation representation;

		IntegerLiteral(int _value, IntegerRepresentation _representation):
			representation(_representation),
			value(_value) { }

		IntegerLiteral(int _value):
			value(_value) { }
	};

	struct FloatLiteral : Expression {
		double value;

		FloatLiteral(double _value):
			value(_value) { }
	};

	enum class ValueTransformType {
		IntegerValue,
		AbsoluteValue,
	};

	struct ValueTransform : Expression {
		ValueTransformType type;

		ValueTransform(ValueTransformType _type):
			type(_type)
		{
			type = _type;
		}
	};

	struct BinaryExpression : Expression {
		Expression left;
		BinaryOperator _operator;
		Expression right;

		BinaryExpression(Expression _left, BinaryOperator __operator, Expression _right):
			left(_left),
			_operator(__operator),
			right(_right)
		{ }
	};
}
#endif //LYA_SYNTAXES_H
