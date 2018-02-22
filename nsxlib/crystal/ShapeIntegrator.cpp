#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "ShapeIntegrator.h"

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

    MillerIndex hkl(peak, *uc);

    StrongPeakIntegrator::compute(peak, region);

    const double mean_bkg = _meanBackground.value();
    const double I_peak = _integratedIntensity.value();

    const auto& qs = region.peakData().qs();
    const auto& counts = region.peakData().counts();

    FitProfile profile(_aabb, _nx, _ny, _nz);

    for (size_t i = 0; i < qs.size(); ++i) {
        const double dI = counts[i]-mean_bkg;
        if (dI > 1e-3*I_peak) {
            profile.addData(qs[i], dI);
        }
    }
    profile.normalize();
    library->addShape(hkl, profile);
    return true;
}

} // end namespace nsx
