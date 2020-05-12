#ifndef PHSTORE_PBARENA_MANAGER_H
#define PHSTORE_PBARENA_MANAGER_H

#include <queue>
#include <mutex>
#include "PBArena.h"
#include "Logger.h"

class PBArenaManager {
public:
    static std::shared_ptr<PBArenaManager> GetInstance() {
        static std::shared_ptr<PBArenaManager> instance(new PBArenaManager());
        return instance;
    }
    
    std::shared_ptr<PBArena> GetPBArena() {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (arenaQueue_.empty()) {
            for (uint32_t i = 0; i < arenaQueueIncrementalSize; ++i) {
                arenaQueue_.push(std::make_shared<PBArena>(arenaBlockSizeInMB));
            }
            LOG(INFO) << "Create " << arenaQueueIncrementalSize << " incremental arena and push.";
        }
        std::shared_ptr<PBArena> arena = arenaQueue_.front();
        arena->Reset();
        arenaQueue_.pop();
        return arena;
    }

    void PutPBArena(std::shared_ptr<PBArena> &arena) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        arenaQueue_.push(arena);
    }
    
    PBArenaManager() {
        for (uint32_t i = 0; i < arenaQueueSize; ++i) {
            std::shared_ptr<PBArena> arena = std::make_shared<PBArena>(arenaBlockSizeInMB);
            PutPBArena(arena);
        }
    }

    ~PBArenaManager() {
        std::queue<std::shared_ptr<PBArena>> empty;
        swap(empty, arenaQueue_);
    }
    
private:
    std::queue<std::shared_ptr<PBArena>> arenaQueue_;       // Arena queue.
    std::mutex queueMutex_;                                 // Mutex for arena queue operations.
    uint32_t arenaBlockSizeInMB = 100;               // Arena block size in MB, default is 100MB.
    const uint32_t arenaQueueSize = 30;              // Initial size.
    const uint32_t arenaQueueIncrementalSize = 10;   // Incremental size
};

#endif
