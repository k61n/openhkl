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
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

void IntegrationParameters::log(const Level& level) const
{
    nsxlog(level, "Integration parameters:");
    nsxlog(level, "peak_end               = ", peak_end);
    nsxlog(level, "bkg_begin              = ", bkg_begin);
    nsxlog(level, "bkg_end                = ", bkg_end);
    nsxlog(level, "neighbour_range_pixels = ", neighbour_range_pixels);
    nsxlog(level, "neighbour_range_frames = ", neighbour_range_frames);
    nsxlog(level, "fit_center             = ", fit_center);
    nsxlog(level, "fit_cov                = ", fit_cov);
    nsxlog(level, "integrator_type        = ", static_cast<int>(integrator_type));
    nsxlog(level, "region_type            = ", static_cast<int>(region_type));
}

IPeakIntegrator::IPeakIntegrator()
    : _meanBackground(), _integratedIntensity(), _rockingCurve(), _handler(nullptr), _params{}
{
}

IPeakIntegrator::~IPeakIntegrator() = default;

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
    std::vector<nsx::Peak3D*> peaks, ShapeCollection* shape_collection, sptrDataSet data,
    int n_numor)
{
    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const Peak3D* peak) {
        return peak->dataSet() != data;
    });
    peaks.erase(it, peaks.end());
    std::ostringstream oss;
    std::string status = "Integrating " + std::to_string(peaks.size()) + " peaks...";
    oss << "Integrating " << peaks.size() << " peaks in numor " << n_numor << " of " << _n_numors;
    nsxlog(Level::Info, "IPeakIntegrator::integrate: integrating ", peaks.size(), " peaks");
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    std::map<Peak3D*, std::unique_ptr<IntegrationRegion>> regions;
    std::map<Peak3D*, bool> integrated;

    for (auto peak : peaks) {
        regions.emplace(std::make_pair(
            peak,
            std::make_unique<IntegrationRegion>(
                peak, _params.peak_end, _params.bkg_begin, _params.bkg_end,
                _params.region_type)));
        integrated.emplace(std::make_pair(peak, false));

        // ignore partials
        auto bb = regions.at(peak).get()->peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0) {
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::InvalidRegion);
        }

        if (hi[0] >= data->nCols() || hi[1] >= data->nRows() || hi[2] >= data->nFrames()) {
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::InvalidRegion);
        }
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
            auto* current_peak = regions.at(peak).get();
            assert(current_peak != regions.end());
            current_peak->updateMask(mask, idx);
        }

        #pragma omp parallel for
        for (auto peak : peaks) {
            auto* current_peak = regions.at(peak).get();
            bool result = current_peak->advanceFrame(current_frame, mask, idx);
            // this allows for partials at end of data
            result |= idx == data->nFrames() - 1;

            // done reading peak data
            if (result && !integrated[peak]) {
                current_peak->peakData().computeStandard();
                try {
                    if (compute(peak, shape_collection, *current_peak)) {
                        peak->updateIntegration(
                            rockingCurve(), meanBackground(), integratedIntensity(),
                            _params.peak_end, _params.bkg_begin, _params.bkg_end);
                    } else {
                        peak->setSelected(false);
                        peak->setRejectionFlag(RejectionFlag::IntegrationFailure);
                    }
                } catch (std::exception& e) {
                    // integration failed...
                    nsxlog(
                        Level::Debug, "IPeakIntegrator::integrate: integration failed: ", e.what());
                    #pragma omp atomic
                    ++nfailures;
                    peak->setSelected(false);
                    peak->setRejectionFlag(RejectionFlag::IntegrationFailure);
                }
                // free memory (important!!)
                current_peak->reset();
                integrated[peak] = true;
            }
        }

        if (_handler) {
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / data->nFrames();
            _handler->setProgress(progress);
        }
    }
    nsxlog(Level::Info, "IPeakIntegrator::integrate: end; ", nfailures, " failures");
}

void IPeakIntegrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

void IPeakIntegrator::setParameters(const IntegrationParameters& params)
{
    _params = params;
    _params.log(Level::Info);
}

void IPeakIntegrator::setNNumors(int n_numors)
{
    _n_numors = n_numors;
}

} // namespace nsx
