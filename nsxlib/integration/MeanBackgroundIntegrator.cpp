
#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MeanBackgroundIntegrator.h"
#include "Peak3D.h"

namespace nsx {

MeanBackgroundIntegrator::MeanBackgroundIntegrator(): IPeakIntegrator()
{

}

bool MeanBackgroundIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    double sum_bkg = 0.0;
    double sum_bkg2 = 0.0;
    size_t nbkg = 0;
 
    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

    // debugging
    auto c = peak->getShape().center();

    double y = 470.0 + (880-470.0) / 400.0 * (c[0]-400.0);

    if (c[1] > y && c[0] > 400) {
        std::cout << "debug peak" << std::endl;
        std::cout << "coords " << c.transpose() << std::endl;
        std::cout << "events " << events.size() << std::endl;
    }

    // TODO: should this be hard-coded??
    if (events.size() < 20) {
        throw std::runtime_error("MeanBackgroundIntegrator::compute(): too few data points");
    }

    // compute initial mean background and error
    for (auto i = 0; i < counts.size(); ++i) {
        if (region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND) {
            continue;
        }
        sum_bkg += counts[i];
        sum_bkg2 += counts[i]*counts[i];
        nbkg++;
    }

    double mean_bkg = sum_bkg / nbkg;
    double var_bkg = (sum_bkg2 - nbkg*mean_bkg*mean_bkg) / (nbkg-1);
    double sigma_bkg = std::sqrt(var_bkg);

    // reject outliers
    for (auto i = 0; i < 20; ++i) {
        sum_bkg = 0;
        sum_bkg2 = 0;
        nbkg = 0;

        for (auto i = 0; i < counts.size(); ++i) {
            if (std::fabs(counts[i]-mean_bkg) > 3*sigma_bkg || region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND) {
                continue;
            }
            sum_bkg += counts[i];
            sum_bkg2 += counts[i]*counts[i];
            nbkg++;
        }

        double old_mean = mean_bkg;
        mean_bkg = sum_bkg / nbkg;
        var_bkg = (sum_bkg2 - nbkg*mean_bkg*mean_bkg) / (nbkg-1);
        sigma_bkg = std::sqrt(var_bkg);

        if (std::fabs((old_mean-mean_bkg)/mean_bkg) < 1e-9) {
            break;
        }
    }

    // note: the variance is the variance of the _estimate of the mean background_
    _meanBackground = Intensity(mean_bkg, var_bkg / nbkg);

    return true;
}

} // end namespace nsx
