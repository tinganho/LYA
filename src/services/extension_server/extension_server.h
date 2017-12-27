
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <types.h>
#include <protos/output/service.grpc.pb.h>
#include <protos/output/service.pb.h>

using namespace std;
using namespace grpc;
using namespace Lya::lib::types;
using namespace Lya::protos;

namespace Lya::services {

	typedef function<tuple<vector<Localization>, vector<Diagnostic>>(const string&, const vector<string>&, uint64_t start_line)> ExtractCallback;
	typedef function<vector<Diagnostic>(const vector<string>&)> CompileCallback;

	class ExtensionServer : public protos::LyaService::Service {
	public:
	    ExtensionServer(
	        string _server_address,
	        ExtractCallback _extract_callback,
	        CompileCallback _compile_callback);
	    void start_server(bool quiet);
	    Status extract(ServerContext *context, const PBExtractRequest *request, PBExtractResponse *response) override;
		Status compile(ServerContext* context, const PBCompileRequest* request, PBCompileResponse* response) override;
	    Status check_availability(ServerContext* context, const PBAvailabilityRequest* request, PBAvailabilityResponse* response) override;

	private:
	    string server_address;
	    ExtractCallback extract_callback;
		CompileCallback compile_callback;
	};

} // Lya::Extension

#endif // SERVER_H
