#pragma once

#include <map>
#include <string>

#include "../kernel/Factory.h"
#include "../kernel/Singleton.h"

namespace nsx {

class PeakValidatorFactory : public Factory<PeakValidator,std::string,const std::map<std::string,double>&>, public Singleton<PeakValidatorFactory,Constructor,Destructor>{

private:

    friend class Constructor<PeakValidatorFactory>;

    friend class Destructor<PeakValidatorFactory>;

    PeakValidatorFactory();

    virtual ~PeakValidatorFactory()=default;
};

} // end namespace nsx
