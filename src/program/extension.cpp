
#include "extension.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <json/json.h>
#include <map>
#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include "diagnostics.h"
#include "extension_client.h"
#include "utils.h"
#include "types.h"


namespace Lya::extension {
	using namespace std;
	using namespace utils;
	using namespace types;
	using namespace diagnostics;

	Extension Extension::create(shared_ptr<Session> session, string extension_file) {

	    auto check_capabilities = [&session, &extension_file](const Json::Value& capabilities) -> void {
	        for (auto const& capability : capabilities) {
	            if (capability.isString() != true) {
	                add_diagnostic(*session, D::Capabilities_must_be_a_string_plural_select_etc);
	                continue;
	            }
	            const string c = capability.asString();
	            if (c != "Plural" &&
	                c != "Select" &&
	                c != "Date" &&
	                c != "Number" &&
	                c != "Ordinal" &&
	                c != "Currency") {

	                add_diagnostic(*session, D::Unknown_capability_0_in_extension_file_1, c, extension_file);
	            }
	        }
	    };

	    Json::Reader reader;
	    Json::Value manifest;
	    bool ok = reader.parse(read_file(extension_file), manifest);
	    if (!ok) {
	        add_diagnostic(*session, D::Could_not_read_your_extension_file_0, "ProgrammingLanguage", extension_file);
	    }
	    Extension extension;
	    auto programming_language = manifest["ProgrammingLanguage"];
	    if (programming_language.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "ProgrammingLanguage", extension_file);
	    }
	    auto file_extensions = manifest["FileExtensions"];
	    if (file_extensions.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "FileExtensions", extension_file);
	    }
	    auto function_names = manifest["FunctionNames"];
	    if (function_names.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "FunctionNames", extension_file);
	    }
	    auto capabilities = manifest["Capabilities"];
	    if (capabilities.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "Capabilities", extension_file);
	    }

	    check_capabilities(capabilities);
	    auto dependency_test = manifest["DependencyTest"];
	    if (dependency_test.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "DependencyTest", extension_file);
	    }
	    auto executable = manifest["Executable"];
	    if (executable.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "Executable", extension_file);
	    }
	    auto test_dir = manifest["TestDirectory"];
	    if (test_dir.isNull()) {
	        add_diagnostic(*session, D::Missing_field_0_in_your_extension_file_1, "TestDirectory", extension_file);
	    }

	    extension.programming_language = programming_language.asString();
	    extension.file_extensions = to_vector_of_strings(file_extensions);
	    extension.function_names = to_vector_of_strings(function_names);
	    extension.capabilities = to_vector_of_strings(capabilities);
	    extension.dependency_test = dependency_test.asString();
	    extension.executable = executable.asString();
	    extension.test_dir = test_dir.asString();
	    extension.session = session;

	    return extension;
	}

	int Extension::start_server() {
	    pid_t cpid = fork();
	    if (cpid == 0) {
	        execl("/bin/bash", "/bin/bash", "-c", executable.c_str(), (char *) 0);
	    }
	    return cpid;
	}

	tuple<FileToLocalizations, vector<Diagnostic>> Extension::get_localizations(const vector<string>& files, uint64_t start_line) {
	    FileToLocalizations file_to_localizations;
		vector<Diagnostic> diagnostics;
	    bool ok = client->sync(files, function_names, file_to_localizations, diagnostics, start_line);
	    if (!ok) {
	        throw logic_error("Could not extract with extension '" + programming_language + "'.");
	    }
	    return make_tuple(file_to_localizations, diagnostics);
	}

	bool Extension::is_available() {
	    client = unique_ptr<ExtensionClient>(new ExtensionClient(CreateChannel("localhost:8888", InsecureChannelCredentials())));
	    return client->is_available();
	}

} // Lya::Extension

