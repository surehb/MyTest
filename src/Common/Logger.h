#ifndef LOGGER_H
#define LOGGER_H

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <sstream>
#include <time.h>
#include <unistd.h>


class DiagnosticsLog {
public:
    DiagnosticsLog(const char* file_name, int line_number);

    std::ostream& Stream();

    ~DiagnosticsLog();

    static void Flush();

    static void SetBufferCapacity(size_t buffer_capacity);

private:
    std::ostringstream line_buffer_;

    static std::mutex mutex_;
    static size_t capacity_;
    static size_t tail_;
    static std::vector<std::string> buffer_;
    static const size_t TIME_BUFFER_SIZE = 30;
};

class Logger {
public:
    static void InitLogger() {
        static Logger instance("", GetCurrentWorkingDir());
    }

private:
    Logger() {}

    Logger(const std::string &prefix, const std::string &path) {
        logWorker_ = g3::LogWorker::createLogWorker();
        g3::initializeLogging(logWorker_.get());

        std::function<void(g3::FatalMessagePtr) > fatal_to_g3logworker_function_ptr =
            [](g3::FatalMessagePtr ptr) {
                DiagnosticsLog::Flush();
                g3::internal::pushFatalMessageToLogger(ptr);
            };
        g3::setFatalExitHandler(fatal_to_g3logworker_function_ptr);
    }

    static std::string GetCurrentWorkingDir() {
        char buf[1024];
        getcwd(buf, 1024);
        return buf;
    }

private:
    const size_t flushPolicy_ = 20;                         // Log count.
    const uint32_t maxLogSizeInBytes_ = 200 * 1024 * 1024;  // 200MB.
    const uint32_t maxArchiveLogFileCount_ = 10;            // Log file count.
    std::unique_ptr<g3::LogWorker> logWorker_;
};

const LEVELS DIAGNOSE{600, {"DIAGNOSE"}};

struct MyLogCapture {

    /**
     * @file, line, function are given in g3log.hpp from macros
     * @level INFO/DEBUG/WARNING/FATAL
     * @expression for CHECK calls
     * @fatal_signal for failed CHECK:SIGABRT or fatal signal caught in the signal handler
     */
    MyLogCapture(const char *file, const int line, const char *function, const LEVELS &level,
            const char *expression = "", g3::SignalType fatal_signal = SIGABRT, const char *dump = nullptr) {
        if (level.value == 600) {
            diagnoseLog_.reset(new DiagnosticsLog(file, line));
        } else {
            logCapture_.reset(new LogCapture(file, line, function, level, expression, fatal_signal, dump));
        }
    }


    std::ostream& Stream() {
        if (diagnoseLog_ != nullptr) {
            return diagnoseLog_->Stream();
        } else {
            return logCapture_->stream();
        }
    }

    std::unique_ptr<DiagnosticsLog> diagnoseLog_;
    std::unique_ptr<LogCapture> logCapture_;
};



#undef LOG
#define LOG(level) if(!g3::logLevel(level)){} else MyLogCapture(__FILE__, __LINE__, static_cast<const char*>(__PRETTY_FUNCTION__), level).Stream()

#endif //LOGGER_H
