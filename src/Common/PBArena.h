#ifndef PBARENA_H
#define PBARENA_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>

#include <google/protobuf/arena.h>

#define MB(x) ((size_t) (x) << 20)

class PBArena {
public:
    PBArena() {
        Init(defaultArenaSize);
    }
    
    PBArena(uint32_t arenaSizeInMB) {
        Init(MB(arenaSizeInMB));
    }

    ~PBArena() {}

    void Reset() {
        arena->Reset();
    }

    template <typename T, typename... Args>
    T* CreateMessage(Args&&... args) {
        return ::google::protobuf::Arena::CreateMessage<T>(arena.get(), std::forward<Args>(args)...);
    }
    
    char* AllocateBuffer(size_t size) {
        return ::google::protobuf::Arena::CreateArray<char>(arena.get(), size);
    }
    
private:
    void Init(uint32_t initArenaSize) {
        buffer_.reset(new char[initArenaSize]);
        ::google::protobuf::ArenaOptions option;
        option.initial_block = buffer_.get();
        option.initial_block_size = initArenaSize;
        arena.reset(new ::google::protobuf::Arena(option));
    }

protected:
    std::unique_ptr<char[]> buffer_;
    std::unique_ptr<::google::protobuf::Arena> arena;
    const uint32_t defaultArenaSize = MB(100);
};

struct PBArenaPool {
    PBArenaPool(uint32_t arenaSizeInMB = 1, uint32_t poolSize = 200)
    : arenaSizeInMB_(arenaSizeInMB), poolSize_(poolSize)
    , leftSize_(poolSize), usedLimit_(poolSize), stop_(false) {
        increasement_ = (usedLimit_ / 10 > 0) ? usedLimit_ / 10 : 1;
    }
    
    ~PBArenaPool() {
        stop_ = true;
        condition.notify_all();
    }
    
    void PutBackArena(const std::shared_ptr<PBArena> &arena) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            arenaQueue_.push_back(arena);
        }
        condition.notify_one();
    }
    
    std::shared_ptr<PBArena> GetArena() {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (usedLimit_ > 0 && arenaQueue_.empty()) {
            for (uint32_t i = 0; i < increasement_; i++) {
                std::shared_ptr<PBArena> arena = std::make_shared<PBArena>(arenaSizeInMB_);
                arenaQueue_.push_back(arena);
            }
            usedLimit_ -= increasement_;
        }
        condition.wait(lock, [this] {
            return stop_ || !this->arenaQueue_.empty();
        });
        if (!stop_) {
            auto arena = *arenaQueue_.rbegin();
            arenaQueue_.pop_back();
            leftSize_ = arenaQueue_.size();
            if (arena != nullptr) {
                arena->Reset();
            }
            return arena;
        } else {
            return nullptr;
        }
    }
    
    std::mutex queueMutex_;
    std::condition_variable condition;
    
    uint32_t arenaSizeInMB_;
    uint32_t poolSize_;
    uint32_t leftSize_;
    uint32_t usedLimit_;
    uint32_t increasement_;
    
    std::atomic<bool> stop_;
    
    std::vector<std::shared_ptr<PBArena>> arenaQueue_;
};

#endif
