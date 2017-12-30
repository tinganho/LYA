//
// Created by Tingan Ho on 11/18/17.
//

#include <libxml++/libxml++.h>
#include "../configurations.h"
#include "utils.h"
#include <cstring>

using namespace Lya::lib::utils;

int main() {
    const vector<char*> definitions = {
        "attributes",
        "folders",
        "localizations",
    };
    for (const auto& definition : definitions) {
        const vector<string> test_cases = find_files(string("src/core/dtd/tests/") + string(definition) + string("/*"), PROJECT_DIR);
        for (const auto& test_case : test_cases) {
            try {
                xmlpp::DomParser parser(test_case);
                xmlpp::DtdValidator validator(string(PROJECT_DIR) + string("src/core/dtd/") + string(definition) + string(".dtd"));
                xmlpp::Document *document = parser.get_document();
                validator.validate(document);
            }
            catch (xmlpp::validity_error& ex) {
                cerr << "Error in file: " << test_case << ":" << endl;
                cerr << ex.what() << endl;
                return 1;
            }
            catch (xmlpp::internal_error& ex) {
                cerr << ex.what() << endl;
                return 1;
            }
        }
    }
    return 0;
}