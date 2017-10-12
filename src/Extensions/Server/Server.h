
#ifndef SERVER_H
#define SERVER_H

#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>
#include <string>
#include <Types.h>

using namespace std;
using namespace grpc;
using namespace Lya::ProtocolBuffers;
using namespace Lya::Types;

namespace Lya::Extension {

typedef function<tuple<vector<Localization>, vector<Diagnostic>>(const string&, const vector<string>&)> ExtractLocalization;

class ExtensionServer : public ProtocolBuffers::LyaService::Service {
public:
    ExtensionServer(
        string _server_address,
        ExtractLocalization _extract_localizations);
    void start_server(bool quiet);
    Status sync(ServerContext* context, const PBSyncRequest* request, PBSyncResponse* response) override;
    Status check_availability(ServerContext* context, const PBAvailabilityRequest* request, PBAvailabilityResponse* response) override;

private:
    string server_address;
    ExtractLocalization extract_localizations;
};

} // Lya::Extension

#endif // SERVER_H
