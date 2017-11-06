
#include "compiler.h"
#include "utils.h"
#include "diagnostics.h"
#include <libxml++/libxml++.h>

using namespace Lya::utils;
using namespace xmlpp;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {
	Compiler::Compiler(): dtd_file("") { }

	vector<Diagnostic> Compiler::compile(const vector<string>& localization_files) {
		vector<Diagnostic> diagnostics {};
		if (dtd_file.empty()) {
			dtd_file = get_dtd_file();
		}
		for (const auto& file : localization_files) {
			try {
				DomParser parser(file);
				DtdValidator validator(dtd_file);
				Document* document = parser.get_document();
				validator.validate(document);
				const Node* root = document->get_root_node();
				const Node::NodeList localizations = root->get_children("Localization");
				for (const auto& l : localizations) {
					const Element* localization = dynamic_cast<Element*>(l);
					if (localization == nullptr) {
						throw logic_error("localization is null.");
					}
					const Attribute* id = localization->get_attribute("id");
					vector<Param> params {};
					const Node* parameters_node = localization->get_first_child("Parameters");
					const Node::NodeList parameters = parameters_node->get_children("Parameter");
					for (const auto& p : parameters) {
						const Element* parameter = dynamic_cast<Element*>(p);
						parameter->get_children("Type");
					}
				}
			}
			catch (validity_error ex) {
			}
		}

		return diagnostics;
	}

	string Compiler::get_dtd_file() {
		cout << get_exec_path() << endl;
		return resolve_paths(get_exec_path(), "../../../../dtd/localizations.dtd");
	}
}