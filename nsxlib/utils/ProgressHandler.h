// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXLIB_PROGRESSHANDLER_H
#define NSXLIB_PROGRESSHANDLER_H

#include <mutex>
#include <functional>
#include <string>
#include <vector>
#include <atomic>

namespace nsx {

//! thread safe class to handle progress and status messages
class ProgressHandler {
public:
    ProgressHandler();
    ~ProgressHandler();

    void setCallback(std::function<void()> callback);

    void setProgress(int progress);
    int getProgress();

    void setStatus(const char* status);
    const std::string getStatus();

    void log(const char* message);
    void log(const std::string& message);
    std::vector<std::string> getLog();

    void abort();
    bool aborted();

private:
    std::mutex _mutex;
    std::string _status;
    std::vector<std::string> _log;
    int _progress;

    std::atomic_bool _aborted;

    std::function<void(void)> _callback;
};

} // end namespace nsx

#endif // NSXLIB_PROGRESSHANDLER_H
