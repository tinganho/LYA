
#include <grpc/grpc.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/channel.h>
#include <ProtocolBuffers/Build/LyaService.grpc.pb.h>
#include <ProtocolBuffers/Build/LyaService.pb.h>

using namespace std;
using namespace grpc;
using namespace lya;

class LyaClient {
public:
    LyaClient(shared_ptr<ChannelInterface> channel) :
        stub_(Lya::NewStub(channel)) {
    }

    bool sync() {
        ClientContext context;
        SyncRequest request;
        SyncResponse response;
        Status status = stub_->sync(&context, request, &response);
        if (!status.ok()) {
            cout << "GetFeature rpc failed." << endl;
            return false;
        }
        auto localizations = response.localizations();
        if (localizations.size() > 0) {
            cout << localizations.size() << endl;
            return true;
        }
        return false;
    }

private:
    unique_ptr<Lya::Stub> stub_;
};

int main(int argc, char** argv) {
    LyaClient client(CreateCustomChannel("localhost:50051", InsecureChannelCredentials(), ChannelArguments()));



    return 0;
};
