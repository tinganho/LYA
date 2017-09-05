
#include <ProtocolBuffers/Build/LyaSync.grpc.pb.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <string>

using namespace std;
using namespace grpc;
using namespace lya;

class LyaService : public Lya::Service {
    Status sync(ServerContext* context, const SyncRequest* request, SyncResponse* response) override {
        cout << request->function_names.at(0) << endl;
        return Status::OK;
    };
};

int main(int argc, char** argv) {
    string server_address = "0.0.0.0:50051";
    LyaService service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;
    server->Wait();
}
