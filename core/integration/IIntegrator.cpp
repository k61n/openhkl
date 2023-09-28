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
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Octree.h"
#include "core/shape/ShapeModel.h"
#include "tables/crystal/UnitCell.h"

#include <algorithm>

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
    ohklLog(level, "max_strength           = ", max_strength);
    ohklLog(level, "max_d                  = ", max_d);
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
    ohklLog(level, "use_max_d              = ", use_max_d);
}

IIntegrator::IIntegrator()
    : _sumBackground()
    , _profileBackground()
    , _meanBkgGradient()
    , _sumIntensity()
    , _profileIntensity()
    , _rockingCurve()
    , _handler(nullptr)
    , _params{}
{
}

IIntegrator::~IIntegrator() = default;

const std::vector<Intensity>& IIntegrator::rockingCurve() const
{
    return _rockingCurve;
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

    bool profile_integration = false;
    if (_params.integrator_type == IntegratorType::Profile1D
        || _params.integrator_type == IntegratorType::Profile3D
        || _params.integrator_type == IntegratorType::Gaussian)
        profile_integration = true;


    size_t idx = 0;
    int num_frames_done = 0;

    std::map<Peak3D*, std::unique_ptr<IntegrationRegion>> regions;
    std::map<Peak3D*, bool> integrated;

    double peak_end, bkg_begin, bkg_end;
    if (_params.region_type == ohkl::RegionType::VariableEllipsoid) {
        peak_end = _params.peak_end;
        bkg_begin = _params.bkg_begin;
        bkg_end = _params.bkg_end;
    } else {
        peak_end = _params.fixed_peak_end;
        bkg_begin = _params.fixed_bkg_begin;
        bkg_end = _params.fixed_bkg_end;
    }

    for (auto peak : peaks) {
        if (!peak->enabled() && peak->isRejectedFor(RejectionFlag::Extinct))
            continue;

        regions.emplace(std::make_pair(
            peak,
            std::make_unique<IntegrationRegion>(
                peak, peak_end, bkg_begin, bkg_end, _params.region_type)));
        integrated.emplace(std::make_pair(peak, false));

        // ignore partials
        auto bb = regions.at(peak)->peakBB();
        auto data = peak->dataSet();
        auto lo = bb.lower();
        auto hi = bb.upper();

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
    int nfailures = 0;
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
            auto* current_peak = regions.at(peak).get();
            // assert(current_peak != regions.end());
            current_peak->updateMask(mask, idx);
        }

#pragma omp parallel for
        for (auto peak : peaks) {
            auto* current_peak = regions.at(peak).get();
            // Check whether the peak intersects a mask
            if (_params.skip_masked) {
                Ellipsoid shape = peak->shape();
                shape.scale(bkg_end);
                for (const auto* mask : data->masks()) {
                    if (mask->collide(shape)) {
                        peak->setRejectionFlag(RejectionFlag::Masked);
                        ++nfailures;
                        continue;
                    }
                }
            }
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

            // Skip strong and low resolution peaks during profile integration
            if (profile_integration && !reintegrate(peak)) {
                result = false;
                integrated[peak] = true;
            }

            // this allows for partials at end of data
            result |= idx == data->nFrames() - 1;

            // done reading peak data
            if (result && !integrated[peak]) {
                current_peak->peakData().standardizeCoords();
                if (compute(peak, shape_model, *current_peak)) {
                    peak->updateIntegration(
                        _rockingCurve, _sumBackground, _profileBackground, _meanBkgGradient,
                        _sumIntensity, _profileIntensity, _params.peak_end, _params.bkg_begin,
                        _params.bkg_end, _params.region_type);
                    if (saturated)
                        peak->setIntegrationFlag(
                            RejectionFlag::SaturatedPixel, _params.integrator_type);
                } else {
#pragma omp atomic
                    ++nfailures;
                    // This is a fallback. The RejectionFlag should have been set by this point.
                    peak->setIntegrationFlag(
                        RejectionFlag::IntegrationFailure, _params.integrator_type);
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

bool IIntegrator::reintegrate(Peak3D* peak)
{
    bool reintegrate = true;
    if (_params.use_max_strength) // Skip strong peaks
        if (peak->sumIntensity().strength() > _params.max_strength)
            reintegrate = false;
    if (_params.use_max_d) // Skip low resolution peaks
        if (peak->d() > _params.max_d)
            reintegrate = false;

    if (!reintegrate) {
        if (peak->sumRejectionFlag() == RejectionFlag::NotRejected) {
            peak->updateIntegration(
                _rockingCurve, peak->sumBackground(), peak->sumBackground(),
                peak->meanBkgGradient(), peak->sumIntensity(), peak->sumIntensity(),
                _params.peak_end, _params.bkg_begin, _params.bkg_end, _params.region_type);
        } else {
            peak->setIntegrationFlag(peak->sumRejectionFlag(), IntegratorType::Profile3D);
        }
    }

    return reintegrate;
}

} // namespace ohkl
