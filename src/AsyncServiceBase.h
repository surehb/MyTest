#ifndef ASYNCSERVICEBASE_H
#define ASYNCSERVICEBASE_H

#include "MyTest.grpc.pb.h"
#include "Common/ThreadPool.h"
#include "AsyncContext/BaseContext.h"

static const uint32_t CONTEXT_NUM = 100;
static const uint32_t WORKER_NUM = 16;
static const uint32_t COMPLETION_QUEUE_NUM = 2;

class AsyncServiceBase {
public:
    AsyncServiceBase() {}

    virtual ~AsyncServiceBase() {
        Shutdown();
    }

    virtual void CreateContext(grpc::ServerCompletionQueue *cq) = 0;

    void Run(const std::string &ip) {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(ip, grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);

        tp_.reset(new ThreadPool(WORKER_NUM * COMPLETION_QUEUE_NUM));

        for (uint32_t i = 0; i < COMPLETION_QUEUE_NUM; i++) {
            completionQueues_.emplace_back(builder.AddCompletionQueue());
        }

        server_ = builder.BuildAndStart();

        std::vector<std::thread> serviceThreads_;
        for (auto& cq : completionQueues_) {
            serviceThreads_.emplace_back(std::bind(&AsyncServiceBase::ServiceThreadFunc, this, cq.get()));
        }

        // Block here until QueryServer is shutdown
        server_->Wait();

        for (auto& t : serviceThreads_) {
            t.join();
        }

        LOG(INFO) << "Service will shut down.";
    }

    void Shutdown() {
        // Shutdown the server
        if (server_) {
            server_->Shutdown();
        }

        // Shutdown all cqs
        for (auto& queue : completionQueues_) {
            queue->Shutdown();
        }

        LOG(INFO)<< "Service has been shut down.";
    }

    void ServiceThreadFunc(grpc::ServerCompletionQueue* cq) {
        LOG(INFO) << "Starting ServiceThreadFunc.";

        for (uint32_t i = 0; i < CONTEXT_NUM; ++i) {
            CreateContext(cq);
        }

        bool ok;
        void* tag;
        while (cq->Next(&tag, &ok)) {
            auto ctx = BaseContext::detag(tag);
            ctx->Process(ok);
        }

        LOG(INFO)<< "ServerCompletionQueue has been shut down.";
    }

    std::unique_ptr<grpc::Server> server_;
    MyTestPB::MyTestService::AsyncService service_;
    std::shared_ptr<ThreadPool> tp_;
    std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> completionQueues_;
};

#endif //ASYNCSERVICEBASE_H
