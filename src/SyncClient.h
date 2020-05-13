#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "MyTest.pb.h"
#include "MyTest.grpc.pb.h"


class SyncClient {
public:
    SyncClient(const std::string &ip) {
        std::shared_ptr<grpc::Channel> channel = SyncClient::CreateRPCChannel(ip);
        stub_ = MyTestPB::MyTestService::NewStub(channel);
    }

    grpc::Status HelloWorld(const std::string &query,
                            MyTestPB::HelloWorldResponse *pResponse) {
        // fill request
        MyTestPB::HelloWorldRequest request;
        request.set_value(query);
        
        // query
        std::unique_ptr<grpc::ClientContext> pContext = SyncClient::CreateClientContext();
        grpc::Status status = stub_->HelloWorld(pContext.get(), request, pResponse);
        if (status.ok()) {
            return status;
        }

        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "RPC call failed.");
    }

private:
    static std::unique_ptr<grpc::ClientContext> CreateClientContext(uint64_t timeout = 20) {
        std::unique_ptr<grpc::ClientContext> pContext(new grpc::ClientContext());
        auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
        pContext->set_deadline(deadline);
        return pContext;
    }

    static std::shared_ptr<grpc::Channel> CreateRPCChannel(const std::string &ip) {
        grpc::ChannelArguments ch_args;
        ch_args.SetMaxReceiveMessageSize(-1);
        std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(ip, grpc::InsecureChannelCredentials(), ch_args);

        if (channel == nullptr) {
            LOG(WARNING) << "Failed to create channel against " << ip;
        }

        return channel;
    }

private:
    std::shared_ptr<MyTestPB::MyTestService::Stub> stub_;
};

#endif // SYNCCLIENT_H
