%module "pynsx"

 //%feature("notabstract") Animal;
 //%feature("notabstract") Gender;

%include "std_shared_ptr.i"

%shared_ptr(Peak3D)

%{
#define SWIG_FILE_WITH_INIT

#include <boost/property_tree/ptree.hpp>
#include <Eigen/Dense>

using Eigen::Vector3d;
using Eigen::Matrix3d;

namespace proptree=boost::property_tree;
namespace property_tree=boost::property_tree;
  //namespace property

#include "instrument/ComponentState.h"
#include "instrument/Component.h"
#include "instrument/Monochromator.h"
#include "instrument/Source.h"

#include "chemistry/Material.h"

#include "geometry/Blob3D.h"
#include "crystal/UnitCell.h"
%}

//%include "data/IData.h"
%include "instrument/ComponentState.h"
%include "instrument/Component.h"
%include "instrument/Monochromator.h"
%include "instrument/Source.h"

%include "geometry/Blob3D.h"

%include "chemistry/Material.h"
%include "crystal/UnitCell.h"


