//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/utils/LogMessenger.cpp
//! @brief     Implements class LogMessenger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/LogMessenger.h"

namespace nsx {

LogMessenger::receiverHandle LogMessenger::addReceiver(LogMessenger::receiver_t rec_ptr)
{
    receiverHandle rec_h = NO_HANDLE;
    std::size_t i = 0;
    for (; i < MSG_RECEIVERS_MAXNR; ++i) {
        if (!_receivers[i]) {
            _receivers[i] = rec_ptr;
            break;
        }
    }

    if (i < MSG_RECEIVERS_MAXNR)
        rec_h = i;

    return rec_h;
}

void LogMessenger::discardReceiver(const LogMessenger::receiverHandle rec_h) {
    if (rec_h >= 0 && rec_h < MSG_RECEIVERS_MAXNR)
        _receivers[rec_h] = nullptr;
}

void LogMessenger::send(const LogMessage& msg) {
    for (std::size_t i = 0; i < MSG_RECEIVERS_MAXNR; ++i) {
        if (_receivers[i])
            _receivers[i](msg);
    }
}

} // namespace nsx
