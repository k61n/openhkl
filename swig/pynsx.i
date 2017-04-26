%module "pynsx"

 //%feature("notabstract") Animal;
 //%feature("notabstract") Gender;

%include "std_shared_ptr.i"

%shared_ptr(Peak3D)

%{
#define SWIG_FILE_WITH_INIT

#include <boost/property_tree/ptree.hpp>

#include "data/IData.h"
#include "crystal/Peak3D.h"
#include "instrument/Diffractometer.h"
#include "instrument/Source.h"
#include "instrument/Component.h"
#include "instrument/ComponentState.h"
%}

//%include "data/IData.h"
%include "instrument/ComponentState.h"

