//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/LogMessenger.h
//! @brief     Defines class LogMessenger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_LOGMESSENGER_H
#define NSX_BASE_UTILS_LOGMESSENGER_H

#include "base/utils/LogLevel.h"

#include <functional>
#include <string>

namespace ohkl {

struct LogMessage {
    Level level = Level::Off;
    std::string header;
    std::string body;
};

// maximum nr of receivers of the logging module
#define MSG_RECEIVERS_MAXNR 5

//! Enables sending messages to a number of registered receivers.
//! Receivers are functions `fn` of type `void fn(const LogMessage&)`.
//! Receivers will be called synchronously, yet the order of calling is _not_
//! guaranteed to be the same as that of registering.
//! The receiver must be registered and discarded by the _caller_, when its lifetime ends.
class LogMessenger {
 public:
    //! receiver type: stand-alone function
    using receiver_t = std::function<void(const LogMessage&)>;
    //! receiver type: member method
    template <class C> using receiver_m_t = void (C::*)(const LogMessage&);

    //! receiver handle: identifier for a receiver
    using receiverHandle = int;
    //! invalid receiver handle
    static const receiverHandle NO_HANDLE = -1;

    //! Registers a receiver and returns a handle.
    //! In case of failure, the handle will be invalid.
    //! The receiver must be discarded by the _caller_, when its lifetime ends.
    //! Usage:
    //!   receiverHandle h = logMessenger.addReceiver(myReceiverFunction);
    //!   logMessenger.discardReceiver(h); // de-register


    receiverHandle addReceiver(receiver_t rec_ptr);

    //! Registers a receiver method of type `void C::method(const LogMessage&)` and returns a
    //! handle. The receiver must be discarded by the _caller_, when its lifetime ends. Usage
    //! (within MyClass):
    //!   receiverHandle h = logMessenger.addReceiver(&MyClass::myReceiverMethod, this);
    //!   logMessenger.discardReceiver(h); // de-register

    template <class C> receiverHandle addReceiver(receiver_m_t<C> rec_m_ptr, C* obj_ptr)
    {
        return addReceiver(std::bind(rec_m_ptr, obj_ptr, std::placeholders::_1));
    }

    //! Discards a receiver (if exists)
    void discardReceiver(const receiverHandle rec_h);

    //! Sends a message to the registered recievers
    void send(const LogMessage& msg);

 private:
    //! Storage array for the receiver pointers (finite size)
    receiver_t _receivers[MSG_RECEIVERS_MAXNR]{nullptr};
};

} // namespace ohkl

#endif // NSX_BASE_UTILS_LOGMESSENGER_H
