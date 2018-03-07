#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "ShapeIntegrator.h"
#include "PeakCoordinateSystem.h"

namespace nsx {

ShapeIntegrator::ShapeIntegrator(const AABB& aabb, int nx, int ny, int nz): StrongPeakIntegrator(), 
    _library(new ShapeLibrary), 
    _aabb(aabb),
    _nx(nx),
    _ny(ny),
    _nz(nz)
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
    const double I_peak = _integratedIntensity.value();

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    FitProfile profile(_aabb, _nx, _ny, _nz);
    PeakCoordinateSystem frame(peak);

    for (size_t i = 0; i < events.size(); ++i) {
        const double dI = counts[i]-mean_bkg;
        if (dI > 1e-4*I_peak) {
            profile.addValue(frame.transform(events[i]), dI);
        }
    }
    if (profile.normalize()) {
        auto c = peak->getShape().center();
        DetectorEvent ev(c[0], c[1], c[2]);
        _library->addShape(ev, profile);
    }
    return true;
}

sptrShapeLibrary ShapeIntegrator::library() const
{
    return _library;
}

} // end namespace nsx
