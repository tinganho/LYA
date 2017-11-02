
#include "extension_server.h"
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include "utils.h"

using namespace std;
using namespace grpc;
using namespace Lya::Types;
using namespace xmlpp;

namespace Lya::Services {
	using namespace Utils;

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
	        cout << "Extension server listening on " << server_address << endl;
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
	        const auto& result = extract_localizations(f, function_names, request->start_line());
	        for (const auto& l : get<vector<Localization>>(result)) {
	            auto localization = file_to_localization->add_localizations();
	            localization->set_id(l.id);
		        auto location = localization->mutable_location();
		        location->set_line(l.location.line);
		        location->set_column(l.location.column);
		        location->set_length(l.location.length);
	            for (const auto& p : l.params) {
	                auto param = localization->add_parameters();
	                param->set_name(p.name);
		            if (p.type != nullptr) {
			            param->set_type(*p.type);
		            }
	            }
	        }
		    for (const auto d : get<vector<Diagnostic>>(result)) {
			    PBDiagnostic* diagnostic = response->add_diagnostics();
			    diagnostic->set_message(d.message);
			    PBLocation* location = diagnostic->mutable_location();
			    SpanLocation l = d.location;
			    location->set_line(l.line);
			    location->set_column(l.column);
			    location->set_length(l.length);
		    }
	    }
	    return Status::OK;
	}

	Status ExtensionServer::compile(ServerContext *context, const PBCompileRequest *request, PBCompileResponse *response) {
		const string& exec_path = get_exec_path();
		try {
			TextReader reader(join_paths(exec_path, "../"));
			reader.is_valid();
		}
		catch(xmlpp::internal_error& ex) {
			return Status(StatusCode::UNKNOWN, ex.what());
		}
		return Status::OK;
	}

	Status ExtensionServer::check_availability(ServerContext *context, const PBAvailabilityRequest *request, PBAvailabilityResponse *response) {
	    return Status::OK;
	}

} // Lya::Extension
