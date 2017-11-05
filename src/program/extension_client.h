
#ifndef EXTENSION_CLIENT_H
#define EXTENSION_CLIENT_H

#include <grpc++/channel.h>
#include <map>
#include <service.grpc.pb.h>
#include <service.pb.h>
#include "types.h"

using namespace std;
using namespace grpc;
using namespace Lya::protocol_buffers;
using namespace Lya::types;

namespace Lya::extension {

class ExtensionClient {
	struct Signature {
		bool is_valid;
		vector<Param> params;
		SpanLocation location;
	};
	typedef map<string, Signature> Signatures;
public:
    ExtensionClient(shared_ptr<ChannelInterface> channel);

    bool sync(const vector<string>& files, const vector<string>& functions, Lya::types::FileToLocalizations& file_to_localizations, vector<Diagnostic>& diagnostics, uint64_t start_line);
    bool is_available();
private:
    unique_ptr<LyaService::Stub> stub;

	void add_diagnostic(vector<Diagnostic>& diagnostics, const SpanLocation& location);
	void add_diagnostic(vector<Diagnostic>& diagnostics, const PBLocation& location);
};

} // Lya::Extension

#endif // EXTENSION_CLIENT_H