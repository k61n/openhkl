#include <math.h>

#include "EnhancedAnnularConvolver.h"
#include "RadialConvolver.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

// M_PI is often defined but not standard
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884
#endif

namespace nsx {

EnhancedAnnularConvolver::EnhancedAnnularConvolver()
: Convolver()
{
    // default values
    _parameters["r1"] = 5;
    _parameters["r2"] = 10;
    _parameters["r3"] = 15;
}

EnhancedAnnularConvolver::EnhancedAnnularConvolver(const std::map<std::string,double>& parameters)
: EnhancedAnnularConvolver()
{
    setParameters(parameters);
}

EnhancedAnnularConvolver::~EnhancedAnnularConvolver()
{
}

const char* EnhancedAnnularConvolver::name() const
{
    return "enhanced annular";
}

RealMatrix EnhancedAnnularConvolver::convolve(const RealMatrix& image)
{
    RadialConvolver radial_convolver_peak({{"r_in",0.0},{"r_out",_parameters.at("r1")}});
    RealMatrix conv_peak = radial_convolver_peak.convolve(image);

    RadialConvolver radial_convolver_bkg({{"r_in",_parameters.at("r2")},{"r_out",_parameters.at("r3")}});
    RealMatrix bkg = radial_convolver_bkg.convolve(image);

    RealMatrix diff2 = (image - bkg).cwiseProduct(image-bkg);
    RealMatrix std = radial_convolver_bkg.convolve(diff2).array().sqrt();

    RealMatrix result = (conv_peak - bkg).array() / std.array();

    return result;
}

} // end namespace nsx

