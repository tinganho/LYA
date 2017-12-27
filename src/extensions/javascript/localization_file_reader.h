//
// Created by Tingan Ho on 2017-12-27.
//

#ifndef LYA_LOCALIZATION_FILE_READER_H
#define LYA_LOCALIZATION_FILE_READER_H

#include <string>
#include <vector>
#include "types.h"

using namespace Lya::lib::types;

namespace Lya::javascript_extension
{
	inline std::vector<Diagnostic> read_localization_files(const std::vector<std::string> localization_files);
}
#endif //LYA_LOCALIZATION_FILE_READER_H
