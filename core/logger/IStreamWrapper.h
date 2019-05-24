#ifndef CORE_LOGGER_ISTREAMWRAPPER_H
#define CORE_LOGGER_ISTREAMWRAPPER_H

#include <functional>
#include <string>

namespace nsx {

class Logger;

class IStreamWrapper {
public:
    //! Constructor
    IStreamWrapper(
        std::function<std::string()> prefix = nullptr,
        std::function<std::string()> suffix = nullptr);

    IStreamWrapper(const IStreamWrapper& other) = delete;

    //! Destructor
    virtual ~IStreamWrapper() = 0;

    IStreamWrapper& operator=(const IStreamWrapper& oter) = delete;

    //! Write a message to the stream
    virtual void print(const std::string& message) = 0;

    virtual void printPrefix() = 0;

    virtual void printSuffix() = 0;

protected:
    std::function<std::string()> _prefix;

    std::function<std::string()> _suffix;
};

} // end namespace nsx

#endif // CORE_LOGGER_ISTREAMWRAPPER_H
