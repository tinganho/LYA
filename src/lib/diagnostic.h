//
// Created by Tingan Ho on 2017-11-09.
//

#ifndef LYA_DIAGNOSTIC_H_H
#define LYA_DIAGNOSTIC_H_H

#include <vector>
#include "utils.h"
#include "types.h"
#include "scanner.h"

using namespace Lya::lib;
using namespace Lya::lib::utils;
using namespace Lya::lib::types;

namespace Lya::lib {
	template<class T>
	class DiagnosticList {
	public:
		void add_diagnostic(DiagnosticTemplate _template);
		void add_diagnostic(DiagnosticTemplate _template, string arg1);
		void add_diagnostic(DiagnosticTemplate _template, string arg1, string arg2);
	protected:
		vector<types::Diagnostic> diagnostics;
		bool has_diagnostics;
	};

	template<class T>
	void DiagnosticList<T>::add_diagnostic(DiagnosticTemplate _template) {
		has_diagnostics = true;
		diagnostics.push_back(create_diagnostic(static_cast<T*>(this)->get_token_location(), _template));
	}

	template<class T>
	void DiagnosticList<T>::add_diagnostic(DiagnosticTemplate _template, string arg1) {
		has_diagnostics = true;
		diagnostics.push_back(create_diagnostic(static_cast<T*>(this)->get_token_location(), _template, arg1));
	}

	template<class T>
	void DiagnosticList<T>::add_diagnostic(DiagnosticTemplate _template, string arg1, string arg2) {
		has_diagnostics = true;
		diagnostics.push_back(create_diagnostic(static_cast<T*>(this)->get_token_location(), _template, arg1, arg2));
	}
} // Lya::lib

#endif //LYA_DIAGNOSTIC_H_H
