#include "localization_file_reader.h"
#include "parsers/message/message_parser.h"
#include <libxml++/libxml++.h>

namespace Lya::javascript_extension {

	using namespace Lya::lib::types;
	using namespace Lya::core::parsers::message;

	std::string get_dtd_file()
	{
		return resolve_paths(get_exec_path(), "../../../../core/dtd/localizations.dtd");
	}

	inline LocalizationMessage get_localization(const xmlpp::Node *localization_node)
	{
		const xmlpp::Element* localization_element = dynamic_cast<const xmlpp::Element*>(localization_node);
		if (localization_element == nullptr)
		{
			throw logic_error("localization is null.");
		}
		const xmlpp::Attribute* id = localization_element->get_attribute("id");
		vector<Parameter> params {};
		const xmlpp::Node* parameters_node = localization_element->get_first_child("Parameters");
		const xmlpp::Node::NodeList parameter_node_list = parameters_node->get_children("Parameter");
		for (const xmlpp::Node* parameter_node : parameter_node_list)
		{
			const xmlpp::Element* parameter_element = dynamic_cast<const xmlpp::Element*>(parameter_node);
			const xmlpp::Node::NodeList type_node_list = parameter_element->get_children("Type");
			bool found_type = false;
			std::string type;
			for (const xmlpp::Node* type_node : type_node_list)
			{
				const xmlpp::Element* type_element = dynamic_cast<const xmlpp::Element*>(type_node);
				std::string type_language = type_element->get_attribute("language")->get_value();
				if (type_language == "JavaScript")
				{
					found_type = true;
					type = type_element->get_child_text()->get_content();
				}
			}
			if (!found_type)
			{
				throw std::logic_error("Could not find type for the language JavaScript");
			}
			Parameter parameter(
				parameter_element->get_attribute("name")->get_value(),
				false /*is_list*/,
				type
			);
			params.push_back(parameter);
		}
		const xmlpp::Element *message = dynamic_cast<const xmlpp::Element*>(localization_element->get_first_child("Message"));
		const xmlpp::TextNode *text = message->get_child_text();
		return LocalizationMessage {
			localization_element->get_attribute("id")->get_value(),
			params,
			text->get_content(),
		};
	}

	std::tuple<std::vector<LocalizationMessage>, std::vector<Diagnostic>> read_localization_files(const std::vector<std::string>& localization_files)
	{
		std::string dtd_file;
		std::vector<Diagnostic> diagnostics;
		std::vector<LocalizationMessage> localizations;
		if (dtd_file.empty())
		{
			dtd_file = get_dtd_file();
		}
		for (const auto& file : localization_files)
		{
			xmlpp::DomParser parser(file);
			xmlpp::DtdValidator validator(dtd_file); // TODO: switch to in-memory parsing of DTD to avoid native OS file lookup
			xmlpp::Document* document = parser.get_document();
			validator.validate(document);
			const xmlpp::Node* root = document->get_root_node();
			const xmlpp::Node::NodeList localization_node_list = root->get_children("Localization");
			for (const xmlpp::Node* localization_node : localization_node_list)
			{
				localizations.push_back(get_localization(localization_node));
			}
		}
		return std::make_tuple(localizations, diagnostics);
	}
}