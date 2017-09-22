
#include "ExtensionClient.h"

#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>
#include "Types.h"

using namespace std;
using Lya::Types::FileToLocalizations;
using Lya::Types::Localization;

namespace Lya {
namespace Extension {

ExtensionClient::ExtensionClient(shared_ptr<ChannelInterface> channel):
    stub(LyaService::NewStub(channel)) {
}

bool ExtensionClient::sync(const vector<string>& files, const vector<string>& functions, FileToLocalizations& file_to_localizations) {
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
                vector<string> params;
                for (const auto& ps : ls.params()) {
                    params.push_back(ps);
                }
                localizations.push_back(Localization(ls.id(), ls.line(), ls.column()));
            }
            file_to_localizations[l.file()] = localizations;
        }
        return true;
    }
    return false;
}

bool ExtensionClient::is_available() {
    ClientContext context;
    PBAvailabilityRequest request;
    PBAvailabilityResponse response;
    Status status = stub->check_availability(&context, request, &response);
    return status.ok();
}

} // Extension
} // Lya
