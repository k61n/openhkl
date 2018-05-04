#pragma once

#include <functional>
#include <map>
#include <string>

namespace nsx {

class ResourcesFactory {

public:

    using callback = std::function<const std::map<std::string,std::string>&()>;

    ResourcesFactory();

    const std::map<std::string,std::string>& create(const std::string& resource_type) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~ResourcesFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
