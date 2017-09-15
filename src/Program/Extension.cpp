
#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "Utils.cpp"
#include "Types.cpp"
#include <json/json.h>
#include "Diagnostics.cpp"
#include <map>
#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include "ExtensionClient.cpp"

using namespace std;
using namespace Lya::Utils;
using namespace Lya::Types;
using namespace Lya::Diagnostics;

namespace Lya {
namespace Extension {

class Extension {
public:
    static Extension* create(Session* session, string extension_file) {

        auto check_capabilities = [&session, &extension_file](const Json::Value& capabilities) -> void {
            for (auto const& capability : capabilities) {
                if (capability.isString() != true) {
                    // TODO: Change this
                    add_diagnostic(session, D::Unknown_capability_in_extension_file);
                    continue;
                }
                const string c = capability.asString();
                if (c != "Plural" &&
                    c != "Select" &&
                    c != "Date" &&
                    c != "Number" &&
                    c != "Ordinal" &&
                    c != "Currency") {

                    add_diagnostic(session, D::Unknown_capability_in_extension_file, c, extension_file);
                }
            }
        };

        Json::Reader reader;
        Json::Value manifest;
        bool ok = reader.parse(read_file(extension_file), manifest);
        if (!ok) {
            // TODO: Change this
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "ProgrammingLanguage", extension_file);
        }
        Extension* extension = new Extension();
        auto programming_language = manifest["ProgrammingLanguage"];
        if (programming_language.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "ProgrammingLanguage", extension_file);
        }
        auto file_extensions = manifest["FileExtensions"];
        if (file_extensions.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "FileExtensions", extension_file);
        }
        auto function_names = manifest["FunctionNames"];
        if (function_names.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "FunctionNames", extension_file);
        }
        auto capabilities = manifest["Capabilities"];
        if (capabilities.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "Capabilities", extension_file);
        }

        check_capabilities(capabilities);
        auto dependency_test = manifest["DependencyTest"];
        if (dependency_test.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "DependencyTest", extension_file);
        }
        auto command = manifest["Command"];
        if (command.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "Command", extension_file);
        }
        auto test_dir = manifest["TestDirectory"];
        if (test_dir.isNull()) {
            add_diagnostic(session, D::Missing_field_in_your_extension_file, "TestDirectory", extension_file);
        }

        extension->programming_language = programming_language.asString();
        extension->file_extensions = to_vector_of_strings(file_extensions);
        extension->function_names = to_vector_of_strings(function_names);
        extension->capabilities = to_vector_of_strings(capabilities);
        extension->dependency_test = dependency_test.asString();
        extension->command = command.asString();
        extension->test_dir = test_dir.asString();
        extension->session = session;

        return extension;
    }

    string programming_language;
    vector<string> file_extensions;
    vector<string> function_names;
    vector<string> capabilities;
    string test_dir;
    string dependency_test;
    string command;
    Session* session;

    int start_server() {
        pid_t cpid = fork();
        if (cpid == 0) {
            execl("/bin/bash", "/bin/bash", "-c", command.c_str(), (char *) 0);
        }
        return cpid;
    }

    FileToLocalizations get_localizations(const vector<string>& files, const vector<string>& function_names) {
        ExtensionClient client(CreateCustomChannel("localhost:8888", InsecureChannelCredentials(), ChannelArguments()));
        FileToLocalizations file_to_localizations;
        bool ok = client.sync(files, function_names, file_to_localizations);
        if (!ok) {
            throw logic_error("Could not sync with extension" + programming_language);
        }
        return file_to_localizations;
    }

    void stop_server() {

    }
};

} // Extension
} // Lya

#endif //EXTENSION_H_
