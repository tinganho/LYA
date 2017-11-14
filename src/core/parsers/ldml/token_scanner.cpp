//
// Created by Tingan Ho on 2017-11-13.
//

#include "token_scanner.h"

namespace Lya::core::parsers::ldml {

	Token TokenScanner::next_token() {
		set_token_start_location();
		while (position < length) {
			ch = curr_char();
			increment_position();

			switch (ch) {
			}
		}
	}
}
