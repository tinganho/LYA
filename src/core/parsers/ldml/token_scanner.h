
#ifndef LYA_TOKEN_SCANNER_H
#define LYA_TOKEN_SCANNER_H

#include <glibmm/ustring.h>
#include "scanner.h"
#include "syntaxes.h"

using namespace Lya::lib;

namespace Lya::core::parsers::ldml {

	class TokenScanner final : public Scanner<LdmlToken> {
	public:
		TokenScanner(const Glib::ustring& text);
		LdmlToken next_token() override;
	};
}


#endif //LYA_TOKEN_SCANNER_H
