#include "external_data.h"
#include "parsers/message/message_parser.h"
#include <libxml++/libxml++.h>
#include "configurations.h"
#include "lib/types.h"
#include "lib/utils.h"

using namespace Lya::lib::types;
using namespace Lya::core::parsers::message;

namespace Lya::lib {

	std::string get_dtd_file()
	{
		return resolve_paths(get_exec_path(), "../../../../core/dtd/localizations.dtd");
	}

	LocalizationMessage get_localization(const xmlpp::Node *localization_node)
	{
		const xmlpp::Element* localization_element = dynamic_cast<const xmlpp::Element*>(localization_node);
		if (localization_element == nullptr) {
			throw logic_error("localization is null.");
		}
		const xmlpp::Attribute* id = localization_element->get_attribute("id");
		vector<Parameter> params {};
		const xmlpp::Node* parameters_node = localization_element->get_first_child("Parameters");
		const xmlpp::Node::NodeList parameter_node_list = parameters_node->get_children("Parameter");
		for (const xmlpp::Node* parameter_node : parameter_node_list) {
			const xmlpp::Element* parameter_element = dynamic_cast<const xmlpp::Element*>(parameter_node);
			const xmlpp::Node::NodeList type_node_list = parameter_element->get_children("Type");
			bool found_type = false;
			std::string type;
			for (const xmlpp::Node* type_node : type_node_list) {
				const xmlpp::Element* type_element = dynamic_cast<const xmlpp::Element*>(type_node);
				std::string type_language = type_element->get_attribute("language")->get_value();
				if (type_language == "JavaScript") {
					found_type = true;
					type = type_element->get_child_text()->get_content();
				}
			}
			if (!found_type) {
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
		if (dtd_file.empty()) {
			dtd_file = get_dtd_file();
		}
		for (const auto& file : localization_files) {
			xmlpp::DomParser parser(file);
			xmlpp::DtdValidator validator(dtd_file); // TODO: switch to in-memory parsing of DTD to avoid native OS file lookup
			xmlpp::Document* document = parser.get_document();
			validator.validate(document);
			const xmlpp::Node* root = document->get_root_node();
			const xmlpp::Node::NodeList localization_node_list = root->get_children("Localization");
			for (const xmlpp::Node* localization_node : localization_node_list) {
				localizations.push_back(get_localization(localization_node));
			}
		}
		return std::make_tuple(localizations, diagnostics);
	}

	std::unique_ptr<PluralInfo> read_plural_info(const std::string& language)
	{
		std::string plural_rule_file = resolve_paths(LYA_CLDR_DIR, "common/supplemental/plurals.xml");
		xmlpp::DomParser parser(plural_rule_file);
		xmlpp::Document* document = parser.get_document();
		const xmlpp::Node* root_node = document->get_root_node();
		auto str = "//pluralRules[contains(@locales, '" + split_string(language, '-')[0] + "')]";
		const xmlpp::NodeSet plural_rules_nodes = root_node->find(str);
		if (plural_rules_nodes.size() == 0) {
			throw logic_error("Could not find plural rules for the language" + language);
		}
		const xmlpp::Node* plural_rules_node = plural_rules_nodes[0];
		const xmlpp::Element* plural_rules_element = dynamic_cast<const xmlpp::Element*>(plural_rules_node);
		std::cout << plural_rules_element->get_attribute("locales")->get_value() << endl;
		const xmlpp::Node::NodeList plural_rule_node_list = plural_rules_node->get_children("pluralRule");
		std::map<types::PluralCategory, std::string> plural_rules;
		std::vector<types::PluralCategory> supported_plural_categories;
		for (const xmlpp::Node* plural_rule_node : plural_rule_node_list) {
			const xmlpp::Element* plural_rule_element = dynamic_cast<const xmlpp::Element*>(plural_rule_node);
			const std::string count = plural_rule_element->get_attribute("count")->get_value();
			types::PluralCategory plural_category;
			if (count == "zero") {
				plural_category = types::PluralCategory::Zero;
			}
			else if (count == "one") {
				plural_category = types::PluralCategory::One;
			}
			else if (count == "two") {
				plural_category = types::PluralCategory::Two;
			}
			else if (count == "few") {
				plural_category = types::PluralCategory::Few;
			}
			else if (count == "many") {
				plural_category = types::PluralCategory::Many;
			}
			else if (count == "other") {
				plural_category = types::PluralCategory::Other;
			}
			supported_plural_categories.push_back(plural_category);
			plural_rules[plural_category] = static_cast<std::string>(plural_rule_element->get_child_text()->get_content());
		}

		std::unique_ptr<PluralInfo> plural_info(new PluralInfo { std::move(plural_rules), supported_plural_categories });
		return plural_info;
	}
}