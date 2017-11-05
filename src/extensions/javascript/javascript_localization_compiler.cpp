
#include "javascript_localization_compiler.h"
#include "utils.h"
#include "diagnostics.h"
#include <libxml++/libxml++.h>

using namespace Lya::utils;
using namespace xmlpp;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {

	Compiler::Compiler() {
		string path = get_exec_path();
		cout << path << endl;
	}

	vector<Diagnostic> Compiler::compile(const vector<string>& localization_file_paths) {
		vector<Diagnostic> diagnostics {};

		for (const auto& l : localization_file_paths) {
			try {
				DomParser parser(l);
				DtdValidator validator(l);
				Document* document = parser.get_document();
				validator.validate(document);
				Node* root = document->get_root_node();
				const Node::NodeList localizations = root->get_children();
				for (const auto& l : localizations) {
					const Element* localization = dynamic_cast<Element*>(l);
					const Attribute* id = localization->get_attribute("id");
					vector<Param> params {};
					const Node::NodeList parameters = localization->get_children("Parameter");
					for (const auto& p : parameters) {
						const Element* parameter = dynamic_cast<Element*>(p);
						parameter->get_children("Type");
						delete parameter;
					}
					delete localization;
					delete id;
				}
				delete root;
				delete document;

			}
			catch (validity_error ex) {
			}
		}

		return diagnostics;
	}
}