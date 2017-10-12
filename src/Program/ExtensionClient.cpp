
#include "ExtensionClient.h"

#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>
#include "Types.h"

using namespace std;
using namespace Lya::Types;

namespace Lya::Extension {

ExtensionClient::ExtensionClient(shared_ptr<ChannelInterface> channel):
    stub(LyaService::NewStub(channel)) {
}

bool ExtensionClient::sync(const vector<string>& files, const vector<string>& functions, FileToLocalizations& file_to_localizations, vector<Diagnostic>& diagnostics) {
    ClientContext context;
    PBSyncRequest request;
    PBSyncResponse response;
    for (const auto& f: files) {
        request.add_files(f);
    }
    for (const auto& f: functions) {
        request.add_functions(f);
    }
    Status status = stub->sync(&context, request, &response);
    if (!status.ok()) {
        return false;
    }
    auto file_to_localizations_response = response.file_to_localizations();
    if (file_to_localizations_response.size() > 0) {
        for (const auto& l : file_to_localizations_response) {
            auto localizations_response = l.localizations();
            vector<Localization> localizations;
            for (const auto& ls : localizations_response) {
                vector<Param> params;
                for (const auto& ps : ls.params()) {
                    params.push_back(Param { ps.name(), ps.is_list(), ps.type() });
                }
                localizations.push_back(Localization { ls.id(), params, ls.line(), ls.column() });
            }
            file_to_localizations[l.file()] = localizations;
        }
    }
	auto diagnostics_response = response.diagnostics();
	if (diagnostics_response.size() > 0) {
		for (const auto& d : diagnostics_response) {
			Location location { d.location().line(), d.location().column() };
			Diagnostic diagnostic { d.message(), location };
			diagnostics.push_back(diagnostic);
		}
	}
    return true;
}

bool ExtensionClient::is_available() {
    ClientContext context;
    PBAvailabilityRequest request;
    PBAvailabilityResponse response;
    Status status = stub->check_availability(&context, request, &response);
    return status.ok();
}

} // Lya::Extension
