//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_PARSER_H
#define LYA_MESSAGE_PARSER_H

#include <string>
#include "tokenizer.h"

namespace Lya::core {
	class MessageParser {

	public:
		MessageParser(const string& file);
		Tokenizer tokenizer;
		void parse(const u32string& text);

	private:
	};
}

#endif //LYA_MESSAGE_PARSER_H
