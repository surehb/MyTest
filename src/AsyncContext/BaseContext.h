#ifndef BASECONTEXT_H
#define BASECONTEXT_H

#include <grpc++/grpc++.h>
#include "../MyTest.pb.h"
#include "../MyTest.grpc.pb.h"

#include "../Common/PBArenaManager.h"
#include "../Common/ThreadPool.h"

class BaseContext {
public:
    BaseContext(grpc::ServerCompletionQueue *cq,
                std::shared_ptr<ThreadPool> &tp)
                : cq_(cq), tp_(tp),
                state_(State::CREATE) {}

    virtual ~BaseContext() {
        if (pbArena_ != nullptr) {
            PBArenaManager::GetInstance()->PutPBArena(pbArena_);
        }
    }

    void Process(bool succeeded) {
        switch (state_) {
            case State::CREATE: {
                CreateNewContext();

                if (!succeeded) {
                    LOG(WARNING) << "Fail event received. Probably the request cannot be decoded.";
                    delete this;
                    return;
                }

                tp_->enqueue(std::bind(&BaseContext::ProcessRequest, this));
                state_ = State::DONE;
                break;
            }
            case State::DONE: {
                if (!succeeded) {
                    LOG(WARNING) << "Fail event received. Probably timeout happened while sending the result back.";
                }

                delete this;
                break;
            }
            default: {
                LOG(FATAL) << "This should never happen.";
                break;
            }
        }
    }

    static void* tag(BaseContext* ctx) {
        return static_cast<void*>(ctx);
    }

    static BaseContext* detag(void* tag) {
        return static_cast<BaseContext*>(tag);
    }

protected:
    virtual BaseContext* CreateNewContext() = 0;
    virtual void ProcessRequest() = 0;

    grpc::ServerCompletionQueue *cq_;
    std::shared_ptr<ThreadPool> tp_;
    grpc::ServerContext ctx_;
    std::shared_ptr<PBArena> pbArena_;

    enum State { CREATE, DONE };
    State state_;
};

#endif // BASECONTEXT_H
