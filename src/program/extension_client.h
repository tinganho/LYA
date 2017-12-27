
#ifndef EXTENSION_CLIENT_H
#define EXTENSION_CLIENT_H

#include <map>
#include <grpc++/channel.h>
#include <protos/output/service.grpc.pb.h>
#include <protos/output/service.pb.h>
#include "types.h"

using namespace std;
using namespace grpc;
using namespace Lya::protos;
using namespace Lya::lib::types;

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

    bool extract(const vector<string> &files, const vector<string> &functions,
                 Lya::lib::types::FileToLocalizations &file_to_localizations, vector<Diagnostic> &diagnostics,
                 uint64_t start_line);
	bool compile(const vector<string> &localization_files, vector<Diagnostic>& diagnostics);
    bool is_available();
private:
    unique_ptr<LyaService::Stub> stub;

	void add_diagnostic(vector<Diagnostic>& diagnostics, const SpanLocation& location);
	void add_diagnostic(vector<Diagnostic>& diagnostics, const PBLocation& location);
};

} // Lya::Extension

#endif // EXTENSION_CLIENT_H