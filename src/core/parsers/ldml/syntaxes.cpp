#include "syntaxes.h"

namespace Lya::core::parsers::ldml {

	void Expression::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}

	void TokenNode::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}

	void IntegerLiteral::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}

	void FloatLiteral::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}

	void ValueTransform::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}

	void BinaryExpression::accept(LdmlNodeVisitor* visitor)
	{
		visitor->visit(this);
	}
}