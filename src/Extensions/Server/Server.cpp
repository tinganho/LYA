
#include "Server.h"

#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>

using namespace std;
using namespace grpc;
using namespace Lya::Types;

namespace Lya::Extension {

ExtensionServer::ExtensionServer(
        string _server_address,
        ExtractLocalization _extract_localizations):
        server_address(move(_server_address)),
        extract_localizations(move(_extract_localizations)) { }

void ExtensionServer::start_server(bool quiet = false) {
    ServerBuilder builder;
    builder.AddListeningPort(server_address, InsecureServerCredentials());
    builder.RegisterService(this);
    unique_ptr<Server> s(builder.BuildAndStart());
    if (!quiet) {
        cout << "Server listening on " << server_address << endl;
    }
    s->Wait();
}

Status ExtensionServer::sync(ServerContext* context, const PBSyncRequest* request, PBSyncResponse* response) {
    PBFileToLocalizations* file_to_localization = response->add_file_to_localizations();

    vector<string> function_names;
    for (const auto& f : request->functions()) {
        function_names.push_back(f);
    }
    for (const auto& f : request->files()) {
        file_to_localization->set_file(f);
        const auto& result = extract_localizations(f, function_names);
        for (const auto& l : get<vector<Localization>>(result)) {
            auto localization = file_to_localization->add_localizations();
            localization->set_id(l.id);
            localization->set_column(l.column);
            localization->set_line(l.line);
            for (const auto& p : l.params) {
                auto param = localization->add_params();
                param->set_name(p.name);
	            if (p.type != nullptr) {
		            param->set_type(*p.type);
	            }
            }
        }
	    for (const auto d : get<vector<Diagnostic>>(result)) {
		    PBDiagnostic* diagnostic = response->add_diagnostics();
		    diagnostic->set_message(d.message);
		    PBLocation location = diagnostic->location();
		    location.set_line(d.location.line);
		    location.set_column(d.location.column);
	    }
    }
    return Status::OK;
}

Status ExtensionServer::check_availability(ServerContext *context, const PBAvailabilityRequest *request, PBAvailabilityResponse *response) {
    return Status::OK;
}

} // Lya::Extension
