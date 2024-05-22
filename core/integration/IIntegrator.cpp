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

#include "base/geometry/Ellipsoid.h"
#include "base/mask/IMask.h"
#include "base/utils/Logger.h"
#include "base/utils/ParallelFor.h"
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Octree.h"
#include "core/shape/ShapeModel.h"
#include "tables/crystal/UnitCell.h"

#include <algorithm>
#include <mutex>

namespace ohkl {

void IntegrationParameters::log(const Level& level) const
{
    ohklLog(level, "Integration parameters:");
    ohklLog(level, "peak_end               = ", peak_end);
    ohklLog(level, "bkg_begin              = ", bkg_begin);
    ohklLog(level, "bkg_end                = ", bkg_end);
    ohklLog(level, "fixed_peak_end         = ", fixed_peak_end);
    ohklLog(level, "fixed_bkg_begin        = ", fixed_bkg_begin);
    ohklLog(level, "fixed_bkg_end          = ", fixed_bkg_end);
    ohklLog(level, "fit_center             = ", fit_center);
    ohklLog(level, "fit_cov                = ", fit_cov);
    ohklLog(level, "integrator_type        = ", static_cast<int>(integrator_type));
    ohklLog(level, "region_type            = ", static_cast<int>(region_type));
    ohklLog(level, "use_gradient           = ", use_gradient);
    ohklLog(level, "fft_gradient           = ", fft_gradient);
    ohklLog(level, "gradient_type          = ", static_cast<int>(gradient_type));
    ohklLog(level, "skip_masked            = ", skip_masked);
    ohklLog(level, "remove_overlaps        = ", remove_overlaps);
    ohklLog(level, "use_max_strength       = ", use_max_strength);
    ohklLog(level, "max_strength           = ", max_strength);
    ohklLog(level, "use_max_d              = ", use_max_d);
    ohklLog(level, "max_d                  = ", max_d);
    ohklLog(level, "use_max_width          = ", use_max_width);
    ohklLog(level, "max_width              = ", max_width);
}

ComputeResult::ComputeResult()
    : integration_flag(RejectionFlag::NotRejected)
    , sum_intensity()
    , profile_intensity()
    , sum_background()
    , profile_background()
    , bkg_gradient()
    , rocking_curve()
    , integrator_type(IntegratorType::PixelSum)
    , shape()
{
}

IIntegrator::IIntegrator()
    : _handler(nullptr)
    , _params{}
    , _thread_parallel(true)
    , _max_threads(8)
{
}

void IIntegrator::integrate(
    std::vector<ohkl::Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data)
{
    _params.log(Level::Info);
    if (shape_model)
        shape_model->parameters()->log(Level::Info);

    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const Peak3D* peak) {
        return peak->dataSet() != data;
    });
    peaks.erase(it, peaks.end());
    std::ostringstream oss;
    oss << "Integrating " << peaks.size() << " peaks";
    ohklLog(Level::Info, "IIntegrator::integrate: integrating ", peaks.size(), " peaks");
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

    _profile_integration = false;
    if (_params.integrator_type == IntegratorType::Profile1D
        || _params.integrator_type == IntegratorType::Profile3D
        || _params.integrator_type == IntegratorType::Gaussian
        || _params.integrator_type == IntegratorType::ISigma)
        _profile_integration = true;


    size_t idx = 0;
    _n_frames_done = 0;

    std::map<Peak3D*, std::unique_ptr<IntegrationRegion>> regions;
    std::map<Peak3D*, bool> integrated;

    if (_params.region_type == ohkl::RegionType::VariableEllipsoid) {
        _peak_end = _params.peak_end;
        _bkg_begin = _params.bkg_begin;
        _bkg_end = _params.bkg_end;
    } else {
        _peak_end = _params.fixed_peak_end;
        _bkg_begin = _params.fixed_bkg_begin;
        _bkg_end = _params.fixed_bkg_end;
    }

    for (auto peak : peaks) {
        if (peak->isRejectedFor(RejectionFlag::Extinct)) {
            if (!peak->enabled())
                continue;
        }

        regions.emplace(std::make_pair(
            peak,
            std::make_unique<IntegrationRegion>(
                peak, _peak_end, _bkg_begin, _bkg_end, _params.region_type)));
        integrated.emplace(std::make_pair(peak, false));

        // ignore partials
        auto bb = regions.at(peak)->peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

        double width = hi[2] - lo[2];
        if (_params.use_max_width && width > _params.max_width)
            peak->setIntegrationFlag(RejectionFlag::TooWide, _params.integrator_type);

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0 || hi[0] >= data->nCols() || hi[1] >= data->nRows()
            || hi[2] >= data->nFrames())
            peak->setIntegrationFlag(RejectionFlag::InvalidRegion, _params.integrator_type);
    }

    // only integrate the peaks with valid integration regions
    ohklLog(Level::Debug, "IIntegrator::integrate: remove invalid regions");
    it = std::remove_if(peaks.begin(), peaks.end(), [&](Peak3D*& p) {
        return regions.find(p) == regions.end();
    });
    peaks.erase(it, peaks.end());

    // check for overlaps if requested
    if (_params.remove_overlaps)
        removeOverlaps(regions);

    ohklLog(Level::Debug, "IIntegrator::integrate: frames loop");
    _n_failures = 0;
    for (idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame, gradient;
        Eigen::MatrixXi mask;
        current_frame = data->transformedFrame(idx);
        if (_params.use_gradient)
            gradient = data->gradientFrame(idx, _params.gradient_type, !_params.fft_gradient);

        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

        for (auto peak : peaks) {
            assert(peak != nullptr);
            auto* current_region = regions.at(peak).get();
            current_region->updateMask(mask, idx);
        }

        for (auto peak : peaks) {
            auto* current_region = regions.at(peak).get();
            // Check whether the peak intersects a mask
            if (_params.skip_masked) {
                Ellipsoid shape = peak->shape();
                shape.scale(_bkg_end);
                for (const auto* mask : data->masks()) {
                    if (mask->collide(shape)) {
                        peak->setMasked();
                        continue;
                    }
                }
            }
            // Check for saturated pixels
            const auto& counts = current_region->peakData().counts();
            double max = 0;
            if (!counts.empty()) // std::max on empty vector segfaults under MacOS
                max = *std::max_element(counts.begin(), counts.end());
            const bool saturated = _params.discard_saturated && (max > _params.max_counts);

            bool result = false;
            if (_params.use_gradient)
                result = current_region->advanceFrame(current_frame, mask, idx, &gradient);
            else
                result = current_region->advanceFrame(current_frame, mask, idx);

            bool reintegrate = true;
            if (_profile_integration) {
                if (_params.use_max_strength && peak->sumIntensity().strength() >
                    _params.max_strength)
                    reintegrate = false;
                if (_params.use_max_d && peak->d() > _params.max_d)
                    reintegrate = false;
            }

            // this allows for partials at end of data
            result |= idx == data->nFrames() - 1;

            // done reading peak data
            if (result && !integrated[peak]) {
                ComputeResult compute_result;
                if (!reintegrate) { // do not reintegrate using profile fitting
                    compute_result.integrator_type = _params.integrator_type;
                    compute_result.rocking_curve = peak->rockingCurve();
                    compute_result.profile_intensity = peak->sumIntensity();
                    compute_result.profile_background = peak->sumBackground();
                    compute_result.integration_flag = peak->sumRejectionFlag();
                    integrated[peak] = true;
                } else { // do the integration
                    current_region->peakData().standardizeCoords();
                    compute_result = compute(peak, shape_model, *current_region);
                }

                if (saturated)
                    compute_result.integration_flag = RejectionFlag::SaturatedPixel;
                if (compute_result.integration_flag != RejectionFlag::NotRejected)
                    ++_n_failures;
                integrated[peak] = true;
                peak->updateIntegration(
                    compute_result, _params.peak_end, _params.bkg_begin, _params.bkg_end,
                    _params.region_type);
                current_region->reset();
            }
        }

        if (_handler) {
            ++_n_frames_done;
            double progress = _n_frames_done * 100.0 / data->nFrames();
            _handler->setProgress(progress);
        }
    }
    ohklLog(Level::Info, "IIntegrator::integrate: end; ", _n_failures, " failures");
}

void IIntegrator::parallelIntegrate(
    std::vector<Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data)
{
    ohklLog(Level::Info, "IIntegrator::parallelIntegrate: integrating ", peaks.size(), " peaks");
    _params.log(Level::Info);
    if (shape_model)
        shape_model->parameters()->log(Level::Info);

    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const Peak3D* peak) {
        return peak->dataSet() != data;
    });
    peaks.erase(it, peaks.end());
    std::ostringstream oss;
    oss << "Integrating " << peaks.size() << " peaks";
    ohklLog(Level::Info, "IIntegrator::parallelIntegrate: integrating ", peaks.size(), " peaks");
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

    _profile_integration = false;
    if (_params.integrator_type == IntegratorType::Profile1D
        || _params.integrator_type == IntegratorType::Profile3D
        || _params.integrator_type == IntegratorType::Gaussian
        || _params.integrator_type == IntegratorType::ISigma)
        _profile_integration = true;


    _n_frames_done = 0;

    std::map<Peak3D*, std::unique_ptr<IntegrationRegion>> regions;
    std::map<Peak3D*, bool> integrated;

    if (_params.region_type == ohkl::RegionType::VariableEllipsoid) {
        _peak_end = _params.peak_end;
        _bkg_begin = _params.bkg_begin;
        _bkg_end = _params.bkg_end;
    } else {
        _peak_end = _params.fixed_peak_end;
        _bkg_begin = _params.fixed_bkg_begin;
        _bkg_end = _params.fixed_bkg_end;
    }

    for (auto peak : peaks) {
        peak->setIntegrationFlag(RejectionFlag::NotRejected, _params.integrator_type, true);
        if (peak->isRejectedFor(RejectionFlag::Extinct)) {
            if (!peak->enabled())
                continue;
        }

        regions.emplace(std::make_pair(
            peak,
            std::make_unique<IntegrationRegion>(
                peak, _peak_end, _bkg_begin, _bkg_end, _params.region_type)));
        integrated.emplace(std::make_pair(peak, false));

        // ignore partials
        auto bb = regions.at(peak)->peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

        double width = hi[2] - lo[2];
        if (_params.use_max_width && width > _params.max_width)
            peak->setIntegrationFlag(RejectionFlag::TooWide, _params.integrator_type);

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0 || hi[0] >= data->nCols() || hi[1] >= data->nRows()
            || hi[2] >= data->nFrames())
            peak->setIntegrationFlag(RejectionFlag::InvalidRegion, _params.integrator_type);
    }

    // only integrate the peaks with valid integration regions
    ohklLog(Level::Debug, "IIntegrator::integrate: remove invalid regions");
    it = std::remove_if(peaks.begin(), peaks.end(), [&](Peak3D*& p) {
        return regions.find(p) == regions.end();
    });
    peaks.erase(it, peaks.end());

    // check for overlaps if requested
    if (_params.remove_overlaps)
        removeOverlaps(regions);

    oss << "Building " << peaks.size() << " integration regions";
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }
    ohklLog(Level::Debug, "IIntegrator::parallelIntegrate: integration region loop");
    std::mutex mut1;

    parallel_for(data->nFrames(), [&](int start, int end) {
        for (int idx = start; idx < end; ++idx) {

            Eigen::MatrixXd current_frame, gradient;
            Eigen::MatrixXi mask;
            current_frame = data->transformedFrame(idx);
            if (_params.use_gradient)
                gradient =
                    data->gradientFrame(idx, _params.gradient_type, !_params.fft_gradient);

            mask.resize(data->nRows(), data->nCols());
            mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

            for (auto peak : peaks)
                regions.at(peak)->updateMask(mask, idx);

            std::vector<std::unique_ptr<PeakData>> peak_data_vector;
            for (auto peak : peaks) {
                auto* current_region = regions.at(peak).get();
                PeakData peak_data(peak);
                if (_params.use_gradient)
                    peak_data =
                        current_region->threadSafeAdvanceFrame(current_frame, mask, idx, &gradient);
                else
                    peak_data =
                        current_region->threadSafeAdvanceFrame(current_frame, mask, idx);
                if (!peak_data.empty())
                    peak_data_vector.push_back(std::make_unique<PeakData>(peak_data));
            }

            {
                const std::lock_guard<std::mutex> lock(mut1);
                for (auto& peak_data : peak_data_vector)
                    regions[peak_data->peak()]->appendPeakData(*peak_data);
            }

            if (_handler) {
                const double progress = _n_frames_done++ * 100.0 / data->nFrames();
                _handler->setProgress(progress);
            }
        }
    }, _thread_parallel, _max_threads);

    if (_handler)
        _handler->setProgress(100);

    _n_peaks_done = 0;
    if (_handler)
        _handler->setProgress(0);

    for (auto peak : peaks) {
        if (_params.skip_masked) {
            Ellipsoid shape = peak->shape();
            shape.scale(_bkg_end);
            for (const auto* mask : data->masks()) {
                if (mask->collide(shape)) {
                    peak->setMasked();
                    continue;
                }
            }
        }
    }

    ohklLog(Level::Debug, "IIntegrator::parallelIntegrate: compute loop");
    _n_failures = 0;
    std::mutex mut2;
    parallel_for(peaks.size(), [&](int start, int end) {
        for (int idx = start; idx < end; ++idx) {
            auto* peak = peaks.at(idx);
            auto* current_region = regions.at(peak).get();
            const auto& counts = current_region->peakData().counts();

            RejectionFlag reject(RejectionFlag::NotRejected);

            if (_params.discard_saturated) {
                double max = 0;
                if (!counts.empty()) // std::max on empty vector segfaults under MacOS
                    max = *std::max_element(counts.begin(), counts.end());
                if (max > _params.max_counts)
                    reject = RejectionFlag::SaturatedPixel;
            }

            if (_params.use_max_width) {
                auto aabb = current_region->peakBB();
                const auto lower = aabb.lower();
                const auto upper = aabb.upper();
                const double width = upper[2] - lower[2];
                if (width > static_cast<double>(_params.max_width))
                    reject = RejectionFlag::TooWide;
            }

            bool reintegrate = true;
            if (_profile_integration) {
                if (_params.use_max_strength
                    && peak->sumIntensity().strength() > _params.max_strength)
                    reintegrate = false;
                if (_params.use_max_d && peak->d() > _params.max_d)
                    reintegrate = false;
            }

            ComputeResult compute_result;
            if (!reintegrate) {
                compute_result.integrator_type = _params.integrator_type;
                compute_result.rocking_curve = peak->rockingCurve();
                compute_result.profile_intensity = peak->sumIntensity();
                compute_result.profile_background = peak->sumBackground();
                compute_result.integration_flag = peak->sumRejectionFlag();
            } else {
                if (reject == RejectionFlag::NotRejected) {
                    current_region->peakData().standardizeCoords();
                    compute_result = compute(peak, shape_model, *current_region);
                } else
                    compute_result.integration_flag = reject;

                if (compute_result.integration_flag != RejectionFlag::NotRejected)
                    ++_n_failures;
            }

            {
                const std::lock_guard<std::mutex> lock(mut2);
                peak->updateIntegration(
                    compute_result, _params.peak_end, _params.bkg_begin, _params.bkg_end,
                    _params.region_type);
                current_region->reset();
            }

            if (_handler) {
                const double progress = _n_peaks_done++ * 100.0 / peaks.size();
                _handler->setProgress(progress);
            }
        }
    }, _thread_parallel, _max_threads);

    if (_handler)
        _handler->setProgress(100);

    ohklLog(Level::Info, "IIntegrator::parallelIntegrate: end; ", _n_failures, " failures");
}

void IIntegrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

void IIntegrator::setParameters(const IntegrationParameters& params)
{
    _params = params;
    ohklLog(Level::Info, "IIntegrator::setParameters");
}

void IIntegrator::removeOverlaps(
    const std::map<Peak3D*, std::unique_ptr<IntegrationRegion>>& regions)
{
    ohklLog(Level::Info, "Integrator::removeOverlaps");

    std::vector<Peak3D*> peaks;
    std::vector<Ellipsoid> ellipsoids;

    // shapes (ellipsoids) have already been generated at this points, stored as IntegrationRegion
    for (const auto& [peak, region] : regions) {
        peaks.push_back(peak);
        ellipsoids.push_back(region->shape());
    }

    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d upper(-lower);

    for (const auto& ellipsoid : ellipsoids) {
        Eigen::Vector3d center = ellipsoid.center();
        for (int idx = 0; idx < 3; ++idx) {
            lower(idx) = std::min(lower(idx), center(idx));
            upper(idx) = std::max(upper(idx), center(idx));
        }
    }

    // build octree
    Octree tree(lower, upper);
    for (std::size_t i = 0; i < ellipsoids.size(); ++i)
        tree.addData(&ellipsoids[i]);

    // handle collisions
    int nrejected = 0;
    for (auto collision : tree.getCollisions()) {
        unsigned int i = collision.first - &ellipsoids[0];
        unsigned int j = collision.second - &ellipsoids[0];
        peaks.at(i)->setIntegrationFlag(RejectionFlag::OverlappingPeak, _params.integrator_type);
        peaks.at(j)->setIntegrationFlag(RejectionFlag::OverlappingPeak, _params.integrator_type);
        nrejected += 2;
    }
    ohklLog(Level::Info, "IIntegrator::removeOverlaps: ", nrejected, " overlapping peaks rejected");
}

} // namespace ohkl
