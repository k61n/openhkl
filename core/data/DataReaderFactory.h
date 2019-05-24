#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace nsx {

class IDataReader;
class DataSet;
class Diffractometer;

/** \brief DataReaderFactory. All IData formats must register their "create"
 * method with the factory in order to choose the correct DataReader at runtime.
 * Reader selection is based on the extension of the datafile.
 *
 */
class DataReaderFactory {

public:
    //! Callback type of the factory
    using callback = std::function<std::shared_ptr<DataSet>(const std::string&, Diffractometer*)>;

    DataReaderFactory();

    std::shared_ptr<DataSet> create(
        const std::string& extension, const std::string& filename,
        Diffractometer* diffractometer) const;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
