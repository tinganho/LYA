
#include "compiler.h"
#include "utils.h"
#include "diagnostics.h"
#include "parsers/message/message_parser.h"
#include <libxml++/libxml++.h>

using namespace Lya::lib::utils;
using namespace Lya::core::parsers::message;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {
	Compiler::Compiler(): dtd_file("") { }

	vector<::types::Diagnostic> Compiler::compile(const vector<string>& localization_files) {
		vector<::types::Diagnostic> diagnostics {};
		if (dtd_file.empty()) {
			dtd_file = get_dtd_file();
		}
		for (const auto& file : localization_files) {
			try {
				xmlpp::DomParser parser(file);
				xmlpp::DtdValidator validator(dtd_file);
				xmlpp::Document* document = parser.get_document();
				validator.validate(document);
				const xmlpp::Node* root = document->get_root_node();
				const xmlpp::Node::NodeList localizations = root->get_children("Localization");
				for (const auto& l : localizations) {
					xmlpp::Element* localization = dynamic_cast<xmlpp::Element*>(l);
					if (localization == nullptr) {
						throw logic_error("localization is null.");
					}
					const xmlpp::Attribute* id = localization->get_attribute("id");
					vector<Param> params {};
					const xmlpp::Node* parameters_node = localization->get_first_child("Parameters");
					const xmlpp::Node::NodeList parameters = parameters_node->get_children("Parameter");
					for (const auto& p : parameters) {
						const xmlpp::Element* parameter = dynamic_cast<xmlpp::Element*>(p);
						parameter->get_children("Type");
					}
					const xmlpp::Element* message = dynamic_cast<xmlpp::Element*>(localization->get_first_child("Message"));
					const xmlpp::TextNode* text = message->get_child_text();
					MessageParser message_parser("en-US");
					message_parser.parse(text->get_content(), "en-US");
				}
			}
			catch (xmlpp::validity_error ex) {
				cerr << ex.what() << endl;
			}
		}

		return diagnostics;
	}

	string Compiler::get_dtd_file() {
		return resolve_paths(get_exec_path(), "../../../../core/dtd/localizations.dtd");
	}
}