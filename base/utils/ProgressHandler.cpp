//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/ProgressHandler.cpp
//! @brief     Implements class ProgressHandler
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <iostream>
#include <stdexcept>

#include "base/utils/ProgressHandler.h"

namespace nsx {

ProgressHandler::ProgressHandler()
{
    _progress = 0;
    _aborted = false;
    _callback = nullptr;
}

void ProgressHandler::setCallback(std::function<void()> callback)
{
    _callback = callback;
}

void ProgressHandler::setProgress(int progress)
{
    if (_callback)
        _callback();

    std::lock_guard<std::mutex> lock(_mutex);

    progress = progress > 100 ? 100 : progress;
    progress = progress < 0 ? 0 : progress;

    if (progress != _progress) {
        _progress = progress;
    }

    if (aborted())
        throw std::runtime_error("Exception: job was aborted!");
}

int ProgressHandler::getProgress()
{
    std::lock_guard<std::mutex> lock(_mutex);
    int progress = _progress;
    return progress;
}

void ProgressHandler::setStatus(const char* status)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _status = std::string(status);
}

std::string ProgressHandler::getStatus()
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::string status = _status;
    return status;
}

void ProgressHandler::log(const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _log.push_back(message);
    }

    // without this the logs are not shown unless setProgress is called
    if (_callback)
        _callback();
}

void ProgressHandler::log(const char* message)
{
    log(std::string{message});
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

} // namespace nsx
