#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include <grpcpp/grpcpp.h>
#include "MyTest.grpc.pb.h"

#include "Common/Logger.h"


class SyncService final : public MyTestPB::MyTestService::Service {
public:
    SyncService() {
    }

    void Run(const std::string &ip) {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(ip, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        LOG(INFO) << "Server is listening on: " << ip.data();
        server->Wait();
    }

    grpc::Status HelloWorld(grpc::ServerContext* context,
                      const MyTestPB::HelloWorldRequest* request,
                      MyTestPB::HelloWorldResponse* response) {
        return servingContext_.HelloWorld(request, response);
    }

private:
    ServingContext servingContext_;
};

#endif // SYNCSERVICE_H
