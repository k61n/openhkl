
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
    if (bkgEvents.size() < 20) {
        throw std::runtime_error("MeanBackgroundIntegrator::compute(): too few data points in background");
    }

    // compute initial mean background and error
    for (auto count: bkgCounts) {
        sum_bkg += count;
        sum_bkg2 += count*count;
    }

    double nbkg = bkgCounts.size();
    double mean_bkg = sum_bkg / nbkg;
    double var_bkg = (sum_bkg2 - nbkg*mean_bkg*mean_bkg) / (nbkg-1);
    double sigma_bkg = std::sqrt(var_bkg);

    // reject outliers
    for (auto i = 0; i < 20; ++i) {
        sum_bkg = 0;
        sum_bkg2 = 0;
        nbkg = 0;

        for (auto count: bkgCounts) {
            if (count < mean_bkg+3*sigma_bkg) {
                sum_bkg += count;
                sum_bkg2 += count*count;
                nbkg += 1;
            }
        }

        double old_mean = mean_bkg;
        mean_bkg = sum_bkg / nbkg;
        var_bkg = (sum_bkg2 - nbkg*mean_bkg*mean_bkg) / (nbkg-1);
        sigma_bkg = std::sqrt(var_bkg);

        if (std::fabs((old_mean-mean_bkg)/mean_bkg) < 1e-5) {
            break;
        }
    }

    _meanBackground = Intensity(mean_bkg, var_bkg);

    return true;
}

} // end namespace nsx
