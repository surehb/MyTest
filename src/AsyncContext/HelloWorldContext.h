#ifndef HELLOWORLDCONTEXT_H
#define HELLOWORLDCONTEXT_H

#include "BaseContext.h"
#include "../MyTest.grpc.pb.h"
#include "../MyTest.h"

class HelloWorldContext : public BaseContext {
public:
    HelloWorldContext(MyTestPB::MyTestService::AsyncService *service,
                      grpc::ServerCompletionQueue *cq,
                      std::shared_ptr<ThreadPool> &tp,
                      MyTest &servingContext)
            : BaseContext(cq, tp), service_(service), responseWriter_(&ctx_), servingContext_(servingContext) {
        service_->RequestHelloWorld(&ctx_, &request_, &responseWriter_, cq_, cq_, this);
    }

    BaseContext* CreateNewContext() override {
        return new HelloWorldContext(service_, cq_, tp_, servingContext_);
    }

    void ProcessRequest() override {
        pbArena_ = PBArenaManager::GetInstance()->GetPBArena();
        MyTestPB::HelloWorldResponse *reply = pbArena_->CreateMessage<MyTestPB::HelloWorldResponse>();

        grpc::Status status = servingContext_.HelloWorld(&request_, reply);
        LOG(INFO) << (status.ok() ? "Succeed" : "Failed");
        responseWriter_.Finish(*reply, status, this);
    }

private:
    MyTestPB::MyTestService::AsyncService *service_;
    MyTestPB::HelloWorldRequest request_;
    grpc::ServerAsyncResponseWriter<MyTestPB::HelloWorldResponse> responseWriter_;
    MyTest &servingContext_;
};

#endif //HELLOWORLDCONTEXT_H
