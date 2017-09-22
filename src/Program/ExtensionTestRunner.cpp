
#include <string>
#include <sstream>
#include <iostream>
#if defined(__APPLE__) || defined(__linux__)
#include <signal.h>
#include <unistd.h>
#endif
#include "Types.h"
#include "Utils.h"
#include "Diagnostics.h"
#include "Extension.h"
#include "Core.h"

using namespace Lya::TestFramework;
using namespace Lya::Utils;
using namespace Lya::Types;

namespace Lya {
namespace Extension {

int child;

void kill_all_processes(int signum) {
#if defined(__APPLE__) || defined(__linux__)
    kill(child, SIGTERM);
#endif
}

void run_extension_tests(Lya::Types::Session& session) {
    string extension_file = join_paths(session.root_dir, "Extension.json");
    Extension* extension = Extension::create(session, extension_file);

    const auto start_extension_server = [&]() -> void {
        child = extension->start_server();
        signal(SIGINT, kill_all_processes);
    };

    const auto for_each_key_extraction_test_file = [&](std::function<void (const string&)> callback) -> void {
        vector<string> compilation_test_files = find_files(session.root_dir + "Tests/Cases/KeyExtractions/*");
        for (auto const& f : compilation_test_files) {
            callback(f);
        }
    };

    if (session.start_server) {
        start_extension_server();
    }
    while (!extension->is_available());
    domain("KeyExtractions");
    for_each_key_extraction_test_file([&](const string& test_file) {
        vector<string> files = { test_file };
        FileToLocalizations file_to_localizations;
        try {
            file_to_localizations = extension->get_localizations(files);
        }
        catch (...) {
            kill(child, SIGTERM);
        }
        Json::Value result;
        for (const auto& fl : file_to_localizations) {
            auto localizations = fl.second;
            Json::Value localizations_json = Json::arrayValue;
            for (const auto& l : fl.second) {
                Json::Value params_json = Json::arrayValue;
                for (const auto& p : l.params) {
                    params_json.append(p);
                }
                Json::Value localization_json;
                localization_json["params"] = params_json;
                localization_json["line"] = l.line;
                localization_json["column"] = l.column;
                localizations_json.append(localization_json);
            }
            Json::Value localization_json = Json::arrayValue;
            result[fl.first] = localizations_json;
        }
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "    ";
        unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        ostringstream string_buffer;
        writer->write(result, &string_buffer);
        string result_string = string_buffer.str();
        result_string = replace_string(result_string, session.root_dir + "Tests/Cases/", "");
        string currents_file_path = replace_string(test_file, "Cases", "Currents");
        currents_file_path = replace_string(currents_file_path, ".js", ".json");
        string currents_dir = currents_file_path.substr(0, currents_file_path.find_last_of("/"));
        recursively_create_dir(currents_dir);
        write_file(replace_string(currents_file_path, "Cases", "Currents"), result_string);
        string reference_file_path = replace_string(currents_file_path, "Currents", "References");
        string reference_string = "";
        if (file_exists(reference_file_path)) {
            reference_string = read_file(reference_file_path);
        }
        string test_name = currents_file_path.substr(currents_file_path.find_last_of("/") + 1);
        test_name = replace_string(test_name, session.root_dir, "");
        test(test_name, [reference_string, result_string](Test* t) {
            if (result_string != reference_string) {
                throw runtime_error("Assertion Error!");
            }
        });
    });
    run_tests();
    print_result();
    kill_all_processes(SIGTERM);
}

} // Extension
} // Lya
