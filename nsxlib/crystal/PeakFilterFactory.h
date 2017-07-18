#pragma once

#include <map>
#include <string>

#include "../kernel/Factory.h"
#include "../kernel/Singleton.h"

#include "IPeakFilter.h"

namespace nsx {

class PeakFilterFactory : public Factory<IPeakFilter,std::string>, public Singleton<PeakFilterFactory,Constructor,Destructor>{

private:

    friend class Constructor<PeakFilterFactory>;

    friend class Destructor<PeakFilterFactory>;

    PeakFilterFactory();

    virtual ~PeakFilterFactory()=default;
};

} // end namespace nsx
