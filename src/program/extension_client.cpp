
#include "extension_client.h"
#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include <protos/output/service.grpc.pb.h>
#include <protos/output/service.pb.h>
#include "diagnostics/diagnostics.h"
#include "types.h"

using namespace std;
using namespace Lya::lib::types;
using namespace Lya::core::diagnostics;
using namespace Lya::protos;

namespace Lya::extension {

	ExtensionClient::ExtensionClient(shared_ptr<ChannelInterface> channel):
	    stub(LyaService::NewStub(channel)) {
	}

	bool ExtensionClient::extract(
		const vector<string> &files,
		const vector<string> &functions,
		FileToLocalizations &file_to_localizations,
		vector<Diagnostic>& diagnostics,
		uint64_t start_line = 0)
	{
	    ClientContext context;
	    PBExtractRequest request;
	    PBExtractResponse response;
	    for (const auto& f: files) {
	        request.add_files(f);
	    }
	    for (const auto& f: functions) {
	        request.add_functions(f);
	    }
		request.set_start_line(start_line);
	    Status status = stub->extract(&context, request, &response);
	    if (!status.ok()) {
	        return false;
	    }
		Signatures signatures;
	    auto file_to_localizations_response = response.file_to_localizations();
	    if (!file_to_localizations_response.empty()) {
	        for (const auto& f : file_to_localizations_response) {
	            auto localizations_response = f.localizations();
	            vector<LocalizationLocation> localizations;
		        bool should_break;
	            for (const auto& lr : localizations_response) {
	                vector<Parameter> params;
		            const string& id = lr.id();
		            auto it = signatures.find(id);
		            if (it != signatures.end()) {
			            auto& signature = it->second;
			            if (!signature.is_valid) {
				            add_diagnostic(diagnostics, signature.location);
				            add_diagnostic(diagnostics, lr.location());
				            continue;
			            }
			            auto parameters = lr.parameters();
			            if (parameters.size() != signature.params.size()) {
				            signature.is_valid = false;
				            add_diagnostic(diagnostics, signature.location);
				            add_diagnostic(diagnostics, lr.location());
				            continue;
			            }
			            else {
				            unsigned int i = 0;
				            for (const auto& p : parameters) {
					            const string& name = p.name();
					            const auto& param = signature.params.at(i);
					            if (name != param.name) {
						            add_diagnostic(diagnostics, lr.location());
						            add_diagnostic(diagnostics, signature.location);
						            signature.is_valid = false;
						            should_break = true;
						            break;
					            }
					            if (param.type == nullptr && p.type().empty()) {
						            add_diagnostic(diagnostics, lr.location());
						            add_diagnostic(diagnostics, signature.location);
						            signature.is_valid = false;
						            should_break = true;
						            break;
					            }
					            if (p.type() != *param.type) {
						            add_diagnostic(diagnostics, lr.location());
						            add_diagnostic(diagnostics, signature.location);
						            signature.is_valid = false;
						            should_break = true;
						            break;
					            }
					            params.emplace_back(name, p.is_list(), p.type());
					            i++;
				            }
			            }
		            }
		            if (should_break) {
			            continue;
		            }
		            const auto& l = lr.location();
	                localizations.push_back(LocalizationLocation { lr.id(), params });
		            Signature s {
			            true,
			            params,
						SpanLocation {
				            static_cast<unsigned int>(l.line()),
				            static_cast<unsigned int>(l.column()),
				            static_cast<unsigned int>(l.length()),
				            static_cast<unsigned int>(l.position()),
						}
		            };
		            signatures.emplace(lr.id(), s);

		            outer_end:;
	            }
	            file_to_localizations[f.file()] = localizations;
	        }
	    }
		auto diagnostics_response = response.diagnostics();
		if (!diagnostics_response.empty()) {
			for (const auto& d : diagnostics_response) {
				PBLocation l = d.location();
				SpanLocation location {
					static_cast<unsigned int>(l.line()),
					static_cast<unsigned int>(l.column()),
					static_cast<unsigned int>(l.length()),
					static_cast<unsigned int>(l.position()), };
				Diagnostic diagnostic { d.message(), location };
				diagnostics.push_back(diagnostic);
			}
		}
	    return true;
	}

	bool ExtensionClient::compile(const vector<string>& localization_files, vector<Diagnostic>& diagnostics) {
		ClientContext context;
		PBCompileRequest request;
		PBCompileResponse response;
		for (const string& f : localization_files) {
			request.add_localization_files(f);
		}
		Status status = stub->compile(&context, request, &response);
		if (!status.ok()) {
			return false;
		}
		return true;
	}

	void ExtensionClient::add_diagnostic(vector<Diagnostic>& diagnostics, const SpanLocation& location) {
		Diagnostic diagnostic {
			D::Unmatched_signature.message_template,
			location
		};
		diagnostics.push_back(diagnostic);
	}

	void ExtensionClient::add_diagnostic(vector<Diagnostic>& diagnostics, const PBLocation& location) {
		Diagnostic diagnostic {
			D::Unmatched_signature.message_template,
			SpanLocation {
				static_cast<unsigned int>(location.line()),
				static_cast<unsigned int>(location.column()),
				static_cast<unsigned int>(location.length()),
				static_cast<unsigned int>(location.position()),
			}
		};
		diagnostics.push_back(diagnostic);
	}

	bool ExtensionClient::is_available() {
	    ClientContext context;
	    PBAvailabilityRequest request;
	    PBAvailabilityResponse response;
	    Status status = stub->check_availability(&context, request, &response);
	    return status.ok();
	}

} // Lya::Extension
