//
// Created by Tingan Ho on 2017-12-27.
//

#include "syntaxes.h"

namespace Lya::core::parsers::message {

	TextMessage::TextMessage(std::string _text): text(_text) { }

	void TextMessage::accept(MessageNodeVisitor* visitor) const
	{
		visitor->visit(this);
	}

	void InterpolationMessage::accept(MessageNodeVisitor* visitor) const
	{
		visitor->visit(this);
	}

	void PluralMessage::accept(MessageNodeVisitor* visitor) const
	{
		visitor->visit(this);
	}

};