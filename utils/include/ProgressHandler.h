// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef PROGRESSHANDLER_H
#define PROGRESSHANDLER_H

#include <mutex>
#include <functional>
#include <string>
#include <vector>

namespace SX {

namespace Utils {

// progress handler callback:
// progressHandler(double progress) -> int
// argument progress is the progress (as a percentage)
//using ProgressHandler = std::function<void(double)>;

//! thread safe class to handle progress and status messages
class ProgressHandler {
public:
    ProgressHandler();
    ~ProgressHandler();

    void setProgress(int progress);
    int getProgress();

    void setStatus(const char* status);
    const std::string getStatus();

    void log(const char* message);
    std::vector<std::string> getLog();

private:
    std::mutex _mutex;
    std::string _status;
    std::vector<std::string> _log;
    int _progress;
};

} // namespace Utils

} // namespace SX


#endif // PROGRESSHANDLER_H
