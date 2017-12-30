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

	struct Expression {
		std::vector<std::shared_ptr<Expression>> children;

		Expression():
			children({})
		{ }
	};

	struct Integer : Expression {
		int value;

		Integer(int _value) {
			value = _value;
		}
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
