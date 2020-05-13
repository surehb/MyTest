#include <chrono>
#include <iostream>
#include <unistd.h>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "MyTest.grpc.pb.h"

#include "Common/Logger.h"
#include "SyncClient.h"

using namespace std;
using namespace grpc;
using namespace MyTestPB;


int main(int argc, char **argv) {
    Logger::InitLogger();

    if (argc != 2) {
        cout << "Please input server address." << endl;
        return 0;
    }

    SyncClient client(argv[1]);
    HelloWorldResponse response;
    Status status = client.HelloWorld("shuoch", &response);
    if (status.ok()) {
        cout<< "Value:" << response.value() << ", USN:" << response.usn() << "." << endl;
    } else {
        cout << "Query failed." << endl;
    }

    return 0;
}
