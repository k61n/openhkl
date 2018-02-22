
#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MeanBackgroundIntegrator.h"

namespace nsx {

MeanBackgroundIntegrator::MeanBackgroundIntegrator(): IPeakIntegrator()
{

}

bool MeanBackgroundIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    double sum_bkg = 0.0;
    double sum_bkg2 = 0.0;
 
    const auto& bkgEvents = region.bkgData().events();
    const auto& bkgCounts = region.bkgData().counts();

    // TODO: should this be hard-coded??
    if (bkgEvents.size() < 5) {
        throw std::runtime_error("MeanBackgroundIntegrator::compute(): too few data points in background");
    }

    // compute mean background and error
    for (auto count: bkgCounts) {
        sum_bkg += count;
        sum_bkg2 += count*count;
    }

    const double Nbkg = bkgCounts.size();
    const double mean_bkg = sum_bkg / Nbkg;
    const double var_bkg = (sum_bkg2 - Nbkg*mean_bkg) / (Nbkg-1);

    _meanBackground = Intensity(mean_bkg, var_bkg / Nbkg);

    return true;
}

} // end namespace nsx
