#ifndef SERVINGCONTEXT_H
#define SERVINGCONTEXT_H

#include <atomic>

#include "MyTest.grpc.pb.h"

class ServingContext {
public:
    static grpc::Status HelloWorld(const MyTestPB::HelloWorldRequest *pRequest,
                                   MyTestPB::HelloWorldResponse *pResponse) {
        pResponse->set_value(pRequest->value() + " - Hello World!");
        pResponse->set_usn(++count);
        return grpc::Status::OK;
    }

private:
    static std::atomic<uint64_t> count;
};

std::atomic<uint64_t> ServingContext::count(0);

#endif // SERVINGCONTEXT_H
