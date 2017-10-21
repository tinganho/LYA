
#ifndef EXTENSION_H
#define EXTENSION_H

#include "Types.h"
#include "ExtensionClient.h"

using namespace std;
using namespace Lya::Types;

namespace Lya::Extension {

class Extension {
public:
    static Extension* create(shared_ptr<Session> session, string extension_file);

    string programming_language;
    vector<string> file_extensions;
    vector<string> function_names;
    vector<string> capabilities;
    string test_dir;
    string dependency_test;
    string executable;
    shared_ptr<Session> session;

    int start_server();
    tuple<FileToLocalizations, vector<Diagnostic>> get_localizations(const vector<string>& files, uint64_t start_line);
    void stop_server();
    bool is_available();

private:
    unique_ptr<ExtensionClient> client;
};

} // Lya::Extension

#endif //EXTENSION_H
