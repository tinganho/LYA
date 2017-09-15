
#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <string>
#include <Types.cpp>
#include "Extractor.cpp"

using namespace std;
using namespace grpc;

namespace Lya {
namespace Extension {

typedef function<vector<Lya::Types::Localization>(string)> ExtractLocalization;
class ExtensionServer : public LyaService::Service {
public:
    ExtensionServer(
        string _server_address,
        ExtractLocalization _extract_localizations) :
        server_address(_server_address),
        extract_localizations(_extract_localizations) { }

    void start_server(bool quiet = false) {
        ServerBuilder builder;
        builder.AddListeningPort(server_address, InsecureServerCredentials());
        builder.RegisterService(this);
        unique_ptr<Server> s(builder.BuildAndStart());
        if (!quiet) {
            cout << "Server listening on " << server_address << endl;
        }
        s->Wait();
    }

    Status sync(ServerContext* context, const SyncRequest* request, SyncResponse* response) override {
        FileToLocalizations* file_to_localization = response->add_file_to_localizations();
        for (const auto& f : request->files()) {
            file_to_localization->set_file(f);
            const auto& localizations = extract_localizations(f);
            for (const auto& l : localizations) {
                auto localization = file_to_localization->add_localizations();
                localization->set_id(l.id);
                localization->set_column(l.column);
                localization->set_line(l.line);
                for (const auto& p : l.params) {
                    localization->add_params(p);
                }
            }
        }
        return Status::OK;
    }

private:
    string server_address;
    ExtractLocalization extract_localizations;
};

} // Extension
} // Lya
