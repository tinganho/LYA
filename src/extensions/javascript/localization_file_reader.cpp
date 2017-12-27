//
// Created by Tingan Ho on 2017-12-27.
//

#include "localization_file_reader.h"
#include "parsers/message/message_parser.h"
#include <libxml++/libxml++.h>
#include <iostream>

namespace Lya::javascript_extension {

	using namespace Lya::lib::types;
	using namespace Lya::core::parsers::message;

	std::string get_dtd_file()
	{
		return resolve_paths(get_exec_path(), "../../../../core/dtd/localizations.dtd");
	}

	inline std::string get_message_text(const xmlpp::Node *localization)
	{
		xmlpp::Element* localization_element = dynamic_cast<xmlpp::Element*>(localization);
		if (localization_element == nullptr)
		{
			throw logic_error("localization is null.");
		}
		const xmlpp::Attribute* id = localization_element->get_attribute("id");
		vector<Param> params {};
		const xmlpp::Node* parameters_node = localization_element->get_first_child("Parameters");
		const xmlpp::Node::NodeList parameters = parameters_node->get_children("Parameter");
		for (const auto& p : parameters)
		{
			const xmlpp::Element* parameter = dynamic_cast<xmlpp::Element*>(p);
			parameter->get_children("Type");
		}
		const xmlpp::Element *message = dynamic_cast<xmlpp::Element*>(localization_element->get_first_child("Message"));
		const xmlpp::TextNode *text = message->get_child_text();
		return text->get_content();
	}

	inline std::vector<Diagnostic> read_localization_files(const vector<string> localization_files)
	{
		std::string dtd_file;
		std::vector<Diagnostic> diagnostics {};
		if (dtd_file.empty())
		{
			dtd_file = get_dtd_file();
		}
		for (const auto& file : localization_files)
		{
			try {
				xmlpp::DomParser parser(file);
				xmlpp::DtdValidator validator(dtd_file);
				xmlpp::Document* document = parser.get_document();
				validator.validate(document);
				const xmlpp::Node* root = document->get_root_node();
				const xmlpp::Node::NodeList localizations = root->get_children("Localization");
				for (const auto& localization : localizations)
				{
					get_message_text(localization);
				}
			}
			catch (xmlpp::validity_error ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		}
		return diagnostics;
	}
}