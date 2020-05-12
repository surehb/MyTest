#ifndef ASYNCSERVICE_H
#define ASYNCSERVICE_H

#include <grpcpp/grpcpp.h>

#include "AsyncContext/HelloWorldContext.h"
#include "AsyncServiceBase.h"
#include "MyTest.h"


class AsyncService : public AsyncServiceBase {
public:
    AsyncService() {
    }

    void CreateContext(grpc::ServerCompletionQueue *cq) override {
        new HelloWorldContext(&service_, cq, tp_, myTest_);
    }

private:
    MyTest myTest_;
};

#endif // ASYNCSERVICE_H
