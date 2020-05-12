#include "Logger.h"
#include <g3log/loglevels.hpp>
#include <sstream>
#include <iomanip>

DiagnosticsLog::DiagnosticsLog(const char* file_name, int line_number) {
    auto now = std::chrono::system_clock::now();
    auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %
            1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);

    std::ostringstream time_ss;
    char time_buffer_[TIME_BUFFER_SIZE];
    if (std::strftime(time_buffer_, TIME_BUFFER_SIZE, "%F %T", &bt) != 0) {
        time_ss << time_buffer_;
    }
    time_ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    line_buffer_ << time_ss.str() << " " << file_name << ":" << line_number << ": ";
}

std::ostream& DiagnosticsLog::Stream() { return line_buffer_; }

DiagnosticsLog::~DiagnosticsLog() {
    std::lock_guard<std::mutex> guard(mutex_);

    if (capacity_ == 0) {
        return;
    }

    if (buffer_.size() < capacity_) {
        buffer_.emplace_back(line_buffer_.str());
    } else {
        buffer_[tail_] = line_buffer_.str();
    }
    tail_++;
    if (tail_ >= capacity_) {
        tail_ = 0;
    }
}

void DiagnosticsLog::Flush() {
    std::lock_guard<std::mutex> guard(mutex_);

    if (buffer_.empty()) {
        return;
    }

    const char *var_value = getenv("DIAGNOSE_LOG_DISABLED");
    if (var_value != nullptr && *var_value != '\0') {
        return;
    }

    LOG(WARNING) << "========== DIAGNOSTICS LOG START ==========";
    // Do not concat all buffered logs to a single string.
    // GLOG may truncate it if the log message is too long.
    size_t head = buffer_.size() == capacity_ ? tail_ : 0;
    size_t i = head;
    do {
        // INFO or lower level is required to print diagnostics log.
        LOG(WARNING) << buffer_[i++];
        if (i == buffer_.size()) {
            i = 0;
        }
    } while (i != head);
    LOG(WARNING) << "========== DIAGNOSTICS LOG END ==========";

    buffer_.clear();
    tail_ = 0;
}

void DiagnosticsLog::SetBufferCapacity(size_t buffer_capacity) {
    std::lock_guard<std::mutex> guard(mutex_);

    if (capacity_ != buffer_capacity) {
        // It's suggested to call this function at application startup.
        // If this function is called while application is running,
        // we do not keep the existing logs in memory.
        buffer_.clear();
        tail_ = 0;
        capacity_ = buffer_capacity;
    }
}

std::mutex DiagnosticsLog::mutex_;
size_t DiagnosticsLog::capacity_ = 1000;
size_t DiagnosticsLog::tail_ = 0;
std::vector<std::string> DiagnosticsLog::buffer_;