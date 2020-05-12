#include <chrono>
#include <iostream>
#include <unistd.h>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "MyTest.grpc.pb.h"

#include "Common/Logger.h"
#include "AsyncService.h"
#include "SyncService.h"

using namespace std;
using namespace grpc;
using namespace MyTestPB;


int main(int argc, char **argv) {
    Logger::InitLogger();

    string ip = "";
    string mode = "";

    int optc;
    while (-1 != (optc = getopt(argc, argv, "M:P:"))) {
        switch (optc) {
            case 'M': {
                mode = optarg;
                break;
            }
            case 'P': {
                ip = optarg;
                break;
            }
            default: {
                break;
            }
        }
    }

    if (ip == "" || !(mode == "S" || mode == "A")) {
        LOG(FATAL) << "Invalid paramter, run like this 'gRPC_demo_service -M S(A) -P 127.0.0.1:35'";
        return 1;
    }

    if (mode == "S") {
        SyncService service;
        service.Run(ip);
    }
    else {
        AsyncService service;
        service.Run(ip);
    }

    return 0;
}
