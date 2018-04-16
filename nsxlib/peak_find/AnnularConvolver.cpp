#include "AnnularConvolver.h"
#include "RadialConvolver.h"

namespace nsx {

AnnularConvolver::AnnularConvolver()
: Convolver()
{
    // default values
    _parameters["r1"] = 5;
    _parameters["r2"] = 10;
    _parameters["r3"] = 15;
}

AnnularConvolver::AnnularConvolver(const std::map<std::string,double>& parameters)
: AnnularConvolver()
{
    setParameters(parameters);
}

AnnularConvolver::~AnnularConvolver()
{
}

const char* AnnularConvolver::name() const
{
    return "annular";
}

RealMatrix AnnularConvolver::convolve(const RealMatrix& image)
{
    RadialConvolver radial_convolver_peak({{"r_in",0.0},{"r_out",_parameters.at("r1")}});
    RealMatrix conv_peak = radial_convolver_peak.convolve(image);

    RadialConvolver radial_convolver_bkg({{"r_in",_parameters.at("r2")},{"r_out",_parameters.at("r3")}});
    RealMatrix conv_bkg = radial_convolver_bkg.convolve(image);

    RealMatrix result = conv_peak - conv_bkg;

    return result;
}

} // end namespace nsx

