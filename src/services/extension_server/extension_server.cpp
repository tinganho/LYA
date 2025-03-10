
#include "extension_server.h"
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <libxml++/libxml++.h>
#include "utils.h"

using namespace std;
using namespace grpc;
using namespace xmlpp;
using namespace Lya::lib::utils;
using namespace Lya::lib::types;

namespace Lya::services {

	ExtensionServer::ExtensionServer(
        string _server_address,
        ExtractCallback _extract_callback,
        CompileCallback _compile_callback):
        server_address(move(_server_address)),
        extract_callback(move(_extract_callback)),
        compile_callback(move(_compile_callback))
    { }

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

	Status ExtensionServer::extract(ServerContext *context, const PBExtractRequest *request, PBExtractResponse *response) {
	    PBFileToLocalizations* file_to_localization = response->add_file_to_localizations();

	    std::vector<Glib::ustring> function_names;
	    for (const auto& f : request->functions()) {
	        function_names.push_back(f);
	    }
	    for (const auto& f : request->files()) {
	        file_to_localization->set_file(f);
	        const auto& result = extract_callback(f, function_names, request->start_line());
	        for (const auto& l : get<vector<LocalizationLocation>>(result)) {
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
		const auto& pb_localization_files = request->localization_files();
		vector<string> localization_files;
		for (const auto& l : pb_localization_files) {
			localization_files.push_back(l);
		}
		compile_callback(localization_files);
		return Status::OK;
	}

	Status ExtensionServer::check_availability(ServerContext *context, const PBAvailabilityRequest *request, PBAvailabilityResponse *response) {
	    return Status::OK;
	}

} // Lya::Extension
