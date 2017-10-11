
#ifndef JAVASCRIPT_LOCALIZATION_EXTRACTOR_H
#define JAVASCRIPT_LOCALIZATION_EXTRACTOR_H

#include <tuple>
#include <Types.h>
#include "JavaScriptTokenScanner.h"

using namespace std;
using Lya::Types::Localization;

namespace Lya::JavaScriptExtension {

	enum class JavaScriptLanguage {
		JavaScript,
		TypeScript,
	};

	struct IdentifierType {
		shared_ptr<string> identifier;
		shared_ptr<string> type;
	};

	class JavaScriptLocalizationExtractor {
	public:
	    JavaScriptLocalizationExtractor(const string _file, const vector<string> _function_names, JavaScriptLanguage language);
	    tuple<vector<Localization>> extract();

	private:
		unsigned int unmatched_open_paren;
	    JavaScriptTokenScanner scanner;
	    Token t;
	    vector<u32string> function_names;
		vector<Diagnostic> diagnostics;
		JavaScriptLanguage language;

	    tuple<vector<Param>, bool> scan_parameter_list();
	    bool is_localization_function_name(const u32string &function_name);
	    bool try_scan_localization_function_accessor(const u32string &accessor, u32string &localization_function_name_placeholder);
	    bool scan_expected(const Token &_t);
		bool is_of_supported_type(const u32string &type);
	    Token next_token();
		Token peek_next_token();
	    u32string get_value() const;
		string get_utf8_value();
		Location get_token_location() const;
		string to_utf8_string(const u32string& str);
		u32string to_u32_string(const string& str);
		tuple<IdentifierType, bool> scan_type_and_or_identifier();
		tuple<IdentifierType, bool> scan_type_and_or_identifier(bool expect_type);
		void add_diagnostic(Location location, DiagnosticTemplate _template);
		void add_diagnostic(Location location, DiagnosticTemplate _template, string arg1);
		void add_diagnostic(Location location, DiagnosticTemplate _template, string arg1, string arg2);
	};

} // Lya::JavaScriptExtension

#endif // JAVASCRIPT_LOCALIZATION_EXTRACTOR_H
