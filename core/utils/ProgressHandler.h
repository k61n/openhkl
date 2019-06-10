//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/ProgressHandler.h
//! @brief     Defines class ProgressHandler
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_PROGRESSHANDLER_H
#define CORE_UTILS_PROGRESSHANDLER_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

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

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

} // namespace nsx

#endif // CORE_UTILS_PROGRESSHANDLER_H
