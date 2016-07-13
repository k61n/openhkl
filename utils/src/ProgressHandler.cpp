// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <stdexcept>

#include "ProgressHandler.h"

namespace SX {

namespace Utils {

ProgressHandler::ProgressHandler()
{
    _progress = 0;
    _aborted = false;
}

SX::Utils::ProgressHandler::~ProgressHandler()
{

}

void SX::Utils::ProgressHandler::setProgress(int progress)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _progress = progress > 100 ? 100 : progress;
    _progress = _progress < 0 ? 0 : _progress;

    if ( aborted() )
        throw std::runtime_error("Exception: job was aborted!");
}

int SX::Utils::ProgressHandler::getProgress()
{
    std::lock_guard<std::mutex> lock(_mutex);
    int progress = _progress;
    return progress;
}

void ProgressHandler::setStatus(const char *status)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _status = std::string(status);
}

const std::string ProgressHandler::getStatus()
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::string status = _status;
    return status;
}

void ProgressHandler::log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _log.push_back(message);
}

void ProgressHandler::log(const char *message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _log.push_back(message);
}

std::vector<std::string> ProgressHandler::getLog()
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::vector<std::string> old_log(_log);
    _log.clear();

    return old_log;
}

void ProgressHandler::abort()
{
    _aborted = true;
}

bool ProgressHandler::aborted()
{
    return _aborted;
}



} // namespace Utils

} // namespace SX
