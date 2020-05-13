#ifndef ASYNCSERVICE_H
#define ASYNCSERVICE_H

#include <grpcpp/grpcpp.h>

#include "AsyncContext/HelloWorldContext.h"
#include "AsyncServiceBase.h"
#include "ServingContext.h"


class AsyncService : public AsyncServiceBase {
public:
    AsyncService() {
    }

    void CreateContext(grpc::ServerCompletionQueue *cq) override {
        new HelloWorldContext(&service_, cq, tp_, servingContext_);
    }

private:
    ServingContext servingContext_;
};

#endif // ASYNCSERVICE_H
