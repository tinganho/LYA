
#ifndef EXTENSION_H
#define EXTENSION_H

#include "types.h"
#include "extension_client.h"

using namespace std;
using namespace Lya::types;

namespace Lya::extension {

class Extension {
public:
    static Extension create(shared_ptr<Session> session, string extension_file);

    string programming_language;
    vector<string> file_extensions;
    vector<string> function_names;
    vector<string> capabilities;
    string test_dir;
    string dependency_test;
    string executable;
    shared_ptr<Session> session;

    int start_server();
    tuple<FileToLocalizations, vector<Diagnostic>> extract(const vector<string> &files, uint64_t start_line);
	vector<Diagnostic> compile(const vector<Localization> localizations);
    bool is_available();

private:
    unique_ptr<ExtensionClient> client;
};

} // Lya::Extension

#endif //EXTENSION_H
