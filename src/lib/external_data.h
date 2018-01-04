//
// Created by Tingan Ho on 2017-12-27.
//

#ifndef LYA_LOCALIZATION_FILE_READER_H
#define LYA_LOCALIZATION_FILE_READER_H

#include <string>
#include <vector>
#include <memory>
#include "types.h"

using namespace Lya::lib::types;

namespace Lya::lib
{
	struct PluralInfo {
		std::unique_ptr<std::map<PluralCategory, std::string>> rules;
		std::unique_ptr<std::vector<PluralCategory>> supported_plural_categories;
	};

	std::tuple<std::vector<LocalizationMessage>, std::vector<Diagnostic>> read_localization_files(const std::vector<std::string>& localization_files);
	std::unique_ptr<PluralInfo> read_plural_info(const std::string& language);
}
#endif //LYA_LOCALIZATION_FILE_READER_H
