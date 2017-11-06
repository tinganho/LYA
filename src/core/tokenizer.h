//
// Created by Tingan Ho on 2017-11-06.
//

#ifndef LYA_MESSAGE_SCANNER_H
#define LYA_MESSAGE_SCANNER_H

#include "scanner.h"

using namespace Lya::lib;

namespace Lya::core {
	class Tokenizer : Scanner {
	public:
		Tokenizer(const string& file);
	};
}

#endif //LYA_MESSAGE_SCANNER_H
