//
// Created by Tingan Ho on 2017-11-12.
//

#ifndef LYA_LDML_H
#define LYA_LDML_H

#include "parser.h"
#include "diagnostic.h"

using namespace Lya::lib;

namespace Lya::core {
	enum class Token {
		And,
		Or,
	};

	class Ldml : public Parser<Ldml, Token> {
	public:
		void parse(const u32string& text);
	};
}

#endif //LYA_LDML_H
