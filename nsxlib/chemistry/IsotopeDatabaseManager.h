#ifndef NSXLIB_ISOTOPEDATABASEMANAGER_H_
#define NSXLIB_ISOTOPEDATABASEMANAGER_H_

#include <map>
#include <string>

#include "Isotope.h"
#include "../kernel/Singleton.h"
#include "../utils/Types.h"

#define BOOST_MINOR BOOST_VERSION/100 % 1000

namespace SX {

namespace Chemistry {

using SX::Kernel::Constructor;
using SX::Kernel::Destructor;
using SX::Kernel::Singleton;

class IsotopeDatabaseManager : public Singleton<IsotopeDatabaseManager,Constructor,Destructor> {

public:

    IsotopeDatabaseManager();

    ~IsotopeDatabaseManager()=default;

    Isotope getIsotope(const std::string& name) const;

    const isotopeDatabase& getDatabase() const;

    isotopeDatabase& getDatabase();

    void loadDatabase(const std::string& filename);

    void saveDatabase(std::string filename="") const;

private:

    isotopeDatabase _database;

    static std::string DatabasePath;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXLIB_ISOTOPEDATABASEMANAGER_H_ */
