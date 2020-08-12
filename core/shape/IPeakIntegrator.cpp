//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/IPeakIntegrator.cpp
//! @brief     Implements class IPeakIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/IPeakIntegrator.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

IPeakIntegrator::IPeakIntegrator()
    : _meanBackground()
    , _integratedIntensity()
    , _rockingCurve()
    , _handler(nullptr)
    , _peak_end{3.0}
    , _bkg_begin{3.0}
    , _bkg_end{6.0}
{
}

IPeakIntegrator::~IPeakIntegrator() { }

Intensity IPeakIntegrator::meanBackground() const
{
    return _meanBackground;
}

Intensity IPeakIntegrator::integratedIntensity() const
{
    return _integratedIntensity;
}

const std::vector<Intensity>& IPeakIntegrator::rockingCurve() const
{
    return _rockingCurve;
}

void IPeakIntegrator::integrate(
    std::vector<nsx::Peak3D*> peaks, ShapeLibrary* shape_library, sptrDataSet data)
{
    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const Peak3D* peak) {
        return peak->dataSet() != data;
    });
    peaks.erase(it, peaks.end());
    std::string status = "Integrating " + std::to_string(peaks.size()) + " peaks...";
    nsxlog(Level::Info, "IPeakIntegrator::integrate: integrating", peaks.size(), "peaks");
    if (_handler) {
        _handler->setStatus(status.c_str());
        _handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    std::map<Peak3D*, IntegrationRegion> regions;
    std::map<Peak3D*, bool> integrated;

    for (auto peak : peaks) {
        try {
            // IntegrationRegion constructor may throw (e.g. peak on boundary of image)
            regions.emplace(
                std::make_pair(peak, IntegrationRegion(peak, _peak_end, _bkg_begin, _bkg_end)));
            integrated.emplace(std::make_pair(peak, false));
        } catch (...) {
            peak->setSelected(false);
            continue;
        }

        // ignore partials
        auto bb = regions[peak].peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0)
            peak->setSelected(false);

        if (hi[0] >= data->nCols() || hi[1] >= data->nRows() || hi[2] >= data->nFrames())
            peak->setSelected(false);
    }

    // only integrate the peaks with valid integration regions
    nsxlog(Level::Debug, "IPeakIntegrator::integrate: remove invalid regions");
    it = std::remove_if(peaks.begin(), peaks.end(), [&](Peak3D*& p) {
        return regions.find(p) == regions.end();
    });
    peaks.erase(it, peaks.end());

    nsxlog(Level::Debug, "IPeakIntegrator::integrate: frames loop");
    int nfailures = 0;
    for (idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame;
        Eigen::MatrixXi mask;
        current_frame = data->transformedFrame(idx);

        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

        for (auto peak : peaks) {
            assert(peak != nullptr);
            assert(regions.find(peak) != regions.end());
            regions[peak].updateMask(mask, idx);
        }

        for (auto peak : peaks) {
            bool result = regions[peak].advanceFrame(current_frame, mask, idx);
            // this allows for partials at end of data
            result |= idx == data->nFrames() - 1;

            // done reading peak data
            if (result && !integrated[peak]) {
                regions[peak].peakData().computeStandard();
                try {
                    if (compute(peak, shape_library, regions[peak])) {
                        peak->updateIntegration(
                            rockingCurve(), meanBackground(), integratedIntensity(), _peak_end,
                            _bkg_begin, _bkg_end);
                    } else {
                        peak->setSelected(false);
                    }
                } catch (std::exception& e) {
                    // integration failed...
                    nsxlog(
                        Level::Debug, "IPeakIntegrator::integrate: integration failed", e.what());
                    ++nfailures;
                    peak->setSelected(false);
                }
                // free memory (important!!)
                regions[peak].reset();
                integrated[peak] = true;
            }
        }

        if (_handler) {
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / data->nFrames();
            _handler->setProgress(progress);
        }
    }
    nsxlog(Level::Info, "IPeakIntegrator::integrate: end;", nfailures, "failures");
}

void IPeakIntegrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

} // namespace nsx
