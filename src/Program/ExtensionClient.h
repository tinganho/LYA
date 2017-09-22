
#ifndef EXTENSION_CLIENT_H
#define EXTENSION_CLIENT_H

#include <grpc++/channel.h>
#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>
#include "Types.h"

using namespace std;
using namespace grpc;
using namespace Lya::ProtocolBuffers;
using namespace Lya::Types;

namespace Lya::Extension {

class ExtensionClient {
public:
    ExtensionClient(shared_ptr<ChannelInterface> channel);

    bool sync(const vector<string>& files, const vector<string>& functions, FileToLocalizations& file_to_localizations);
    bool is_available();
private:
    unique_ptr<LyaService::Stub> stub;

};

} // Lya::Extension

#endif // EXTENSION_CLIENT_H