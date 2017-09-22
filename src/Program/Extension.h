
#ifndef EXTENSION_H
#define EXTENSION_H

#include "Types.h"
#include "ExtensionClient.h"

using namespace std;
using namespace Lya::Types;

namespace Lya::Extension {

class Extension {
public:
    static Extension* create(Session& session, string extension_file);

    string programming_language;
    vector<string> file_extensions;
    vector<string> function_names;
    vector<string> capabilities;
    string test_dir;
    string dependency_test;
    string executable;
    unique_ptr<Session> session;

    int start_server();
    FileToLocalizations get_localizations(const vector<string>& files);
    void stop_server();
    bool is_available();

private:
    unique_ptr<ExtensionClient> client;
};

} // Lya::Extension

#endif //EXTENSION_H
