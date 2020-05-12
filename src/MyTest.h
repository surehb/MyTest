#ifndef MYTEST_H
#define MYTEST_H

#include <atomic>

#include "MyTest.grpc.pb.h"

class MyTest {
public:
    static grpc::Status HelloWorld(const MyTestPB::HelloWorldRequest *pRequest,
                                   MyTestPB::HelloWorldResponse *pResponse) {
        pResponse->set_value("Hello World!");
        pResponse->set_usn(++count);
        return grpc::Status::OK;
    }

private:
    static std::atomic<uint64_t> count;
};

std::atomic<uint64_t> MyTest::count(0);

#endif // MYTEST_H
