//
// Created by Tingan Ho on 2017-12-27.
//

#include "syntaxes.h"

namespace Lya::core::parsers::message {

	TextMessage::TextMessage(u32string _text): text(_text) { }

	void TextMessage::accept(Visitor *visitor)
	{
		visitor->visit(this);
	}

	void InterpolationMessage::accept(Visitor *visitor)
	{
		visitor->visit(this);
	}

	void PluralMessage::accept(Visitor *visitor)
	{
		visitor->visit(this);
	}
};