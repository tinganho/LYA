
#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include <Build/LyaService.grpc.pb.h>
#include <Build/LyaService.pb.h>

using namespace std;
using namespace grpc;

namespace Lya {
namespace Extension {

struct Localization {
    string id;
    string text;
    vector<string> params;
    uint64_t line;
    uint64_t column;
    string file_name;

    Localization(const string& _id, const string& _text, const vector<string>& _params, uint64_t _line, uint64_t _column) :
        id(_id),
        text(_text),
        params(_params),
        line(_line),
        column(_column) { }
};

typedef std::map<string, vector<Localization>> FileToLocalizations;

class ExtensionClient {
public:
    ExtensionClient(shared_ptr<ChannelInterface> channel) :
        stub_(LyaService::NewStub(channel)) {
    }

    bool sync(const vector<string>& files, const vector<string>& functions, FileToLocalizations& file_to_localizations) {
        ClientContext context;
        SyncRequest request;
        SyncResponse response;
        Status status = stub_->sync(&context, request, &response);
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
                    localizations.push_back(Localization(ls.id(), ls.text(), params, ls.line(), ls.column()));
                }
                file_to_localizations[l.file()] = localizations;
            }
            return true;
        }
        return false;
    }

private:
    unique_ptr<LyaService::Stub> stub_;
};

} // Extension
} // Lya
