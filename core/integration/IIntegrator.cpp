//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/IIntegrator.cpp
//! @brief     Implements class IIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/IIntegrator.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

#include <algorithm>

namespace ohkl {

void IntegrationParameters::log(const Level& level) const
{
    ohklLog(level, "Integration parameters:");
    ohklLog(level, "peak_end               = ", peak_end);
    ohklLog(level, "bkg_begin              = ", bkg_begin);
    ohklLog(level, "bkg_end                = ", bkg_end);
    ohklLog(level, "neighbour_range_pixels = ", neighbour_range_pixels);
    ohklLog(level, "neighbour_range_frames = ", neighbour_range_frames);
    ohklLog(level, "fit_center             = ", fit_center);
    ohklLog(level, "fit_cov                = ", fit_cov);
    ohklLog(level, "integrator_type        = ", static_cast<int>(integrator_type));
    ohklLog(level, "region_type            = ", static_cast<int>(region_type));
    ohklLog(level, "use_gradient           = ", use_gradient);
    ohklLog(level, "fft_gradient           = ", fft_gradient);
    ohklLog(level, "gradient_type          = ", static_cast<int>(gradient_type));
}

IIntegrator::IIntegrator()
    : _meanBackground(), _integratedIntensity(), _rockingCurve(), _handler(nullptr), _params{}
{
}

IIntegrator::~IIntegrator() = default;

Intensity IIntegrator::meanBackground() const
{
    return _meanBackground;
}

Intensity IIntegrator::meanBkgGradient() const
{
    return _meanBkgGradient;
}

Intensity IIntegrator::integratedIntensity() const
{
    return _integratedIntensity;
}

const std::vector<Intensity>& IIntegrator::rockingCurve() const
{
    return _rockingCurve;
}

void IIntegrator::integrate(
    std::vector<ohkl::Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data, int n_numor)
{
    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const Peak3D* peak) {
        return peak->dataSet() != data;
    });
    peaks.erase(it, peaks.end());
    std::ostringstream oss;
    oss << "Integrating " << peaks.size() << " peaks in numor " << n_numor << " of " << _n_numors;
    ohklLog(Level::Info, "IIntegrator::integrate: integrating ", peaks.size(), " peaks");
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    std::map<Peak3D*, std::unique_ptr<IntegrationRegion>> regions;
    std::map<Peak3D*, bool> integrated;

    for (auto peak : peaks) {
        if (!peak->enabled())
            continue;

        regions.emplace(std::make_pair(
            peak,
            std::make_unique<IntegrationRegion>(
                peak, _params.peak_end, _params.bkg_begin, _params.bkg_end, _params.region_type)));
        integrated.emplace(std::make_pair(peak, false));

        // ignore partials
        auto bb = regions.at(peak)->peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0 || hi[0] >= data->nCols() || hi[1] >= data->nRows()
            || hi[2] >= data->nFrames()) {
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::InvalidRegion);
        }
    }

    // only integrate the peaks with valid integration regions
    ohklLog(Level::Debug, "IIntegrator::integrate: remove invalid regions");
    it = std::remove_if(peaks.begin(), peaks.end(), [&](Peak3D*& p) {
        return regions.find(p) == regions.end();
    });
    peaks.erase(it, peaks.end());

    ohklLog(Level::Debug, "IIntegrator::integrate: frames loop");
    int nfailures = 0;
    for (idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame, gradient;
        Eigen::MatrixXi mask;
        current_frame = data->transformedFrame(idx);
        if (_params.use_gradient)
            gradient = data->imageGradient(
                idx, _params.gradient_type, !_params.fft_gradient)->magnitude();

        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

        for (auto peak : peaks) {
            assert(peak != nullptr);
            auto* current_peak = regions.at(peak).get();
            // assert(current_peak != regions.end());
            current_peak->updateMask(mask, idx);
        }

#pragma omp parallel for
        for (auto peak : peaks) {
            auto* current_peak = regions.at(peak).get();
            // Check for saturated pixels
            const auto& counts = current_peak->peakData().counts();
            double max = 0;
            if (!counts.empty()) // std::max on empty vector segfaults under MacOS
                max = *std::max_element(counts.begin(), counts.end());
            bool saturated = _params.discard_saturated && (max > _params.max_counts);

            bool result;
            if (_params.use_gradient)
                result = current_peak->advanceFrame(current_frame, mask, idx, gradient);
            else
                result = current_peak->advanceFrame(current_frame, mask, idx);
            // this allows for partials at end of data
            result |= idx == data->nFrames() - 1;

            // done reading peak data
            if (result && !integrated[peak]) {
                current_peak->peakData().standardizeCoords();
                if (compute(peak, shape_model, *current_peak)) {
                    peak->updateIntegration(
                        rockingCurve(), meanBackground(), meanBkgGradient(), integratedIntensity(),
                        _params.peak_end, _params.bkg_begin, _params.bkg_end);
                    if (saturated) {
                        peak->setSelected(false);
                        peak->setRejectionFlag(RejectionFlag::SaturatedPixel);
                    }
                } else {
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
    ohklLog(Level::Info, "IIntegrator::integrate: end; ", nfailures, " failures");
}

void IIntegrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

void IIntegrator::setParameters(const IntegrationParameters& params)
{
    _params = params;
    _params.log(Level::Info);
}

void IIntegrator::setNNumors(int n_numors)
{
    _n_numors = n_numors;
}

} // namespace ohkl
