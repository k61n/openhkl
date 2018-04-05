#include "DataSet.h"
#include "Ellipsoid.h"
#include "IntegratedProfile.h"
#include "Intensity.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "ShapeIntegrator.h"
#include "PeakCoordinateSystem.h"

namespace nsx {

ShapeIntegrator::ShapeIntegrator(const AABB& aabb, int nx, int ny, int nz, bool detector_space): StrongPeakIntegrator(), 
    _library(new ShapeLibrary), 
    _aabb(aabb),
    _nx(nx),
    _ny(ny),
    _nz(nz),
    _detectorSpace(detector_space)
{

}

bool ShapeIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    auto uc = peak->activeUnitCell();
    auto data = peak->data();

    if (!uc || !data) {
        throw std::runtime_error("ShapeIntegrator: Peak must have unit cell and data attached");        
    }

    StrongPeakIntegrator::compute(peak, region);

    const double mean_bkg = _meanBackground.value();
    const double std_bkg = _meanBackground.sigma();
    const double I_peak = _integratedIntensity.value();

    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

    FitProfile profile(_aabb, _nx, _ny, _nz);
    // todo: don't use default constructor!
    IntegratedProfile integrated_profile(_meanBackground);
    PeakCoordinateSystem frame(peak);

    Ellipsoid e = peak->getShape();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];
        Eigen::Vector3d x(ev._px, ev._py, ev._frame);              
        const double dI = counts[i]-mean_bkg;
        // todo: variance here assumes Poisson (no gain or baseline)
        integrated_profile.addPoint(e.r2(x), counts[i]);
        
        if (_detectorSpace) {
            x -= peak->getShape().center();
            profile.addValue(x, dI);
        } else {
            profile.addValue(frame.transform(ev), dI);
        }
    }
    if (profile.normalize()) {
        _library->addPeak(peak, std::move(profile), std::move(integrated_profile));
    }
    return true;
}

sptrShapeLibrary ShapeIntegrator::library() const
{
    return _library;
}

} // end namespace nsx
