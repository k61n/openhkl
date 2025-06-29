//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/IntegrationRegion.cpp
//! @brief     Implements class IntegrationRegion
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/IntegrationRegion.h"

#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakData.h"
#include "tables/crystal/UnitCell.h"

namespace ohkl {

IntegrationRegion::IntegrationRegion(
    Peak3D* peak, double peak_end, double bkg_begin, double bkg_end,
    RegionType region_type /* = RegionType::VariableEllipsoid */)
    : _shape(peak->shape())
    , _bkgBegin(bkg_begin)
    , _bkgEnd(bkg_end)
    , _data(peak)
    , _regionType(region_type)
    , _peak(peak)
    , _valid(true)
{
    if (_shape.aabb().lower().hasNaN() || _shape.aabb().upper().hasNaN()) {
        peak->setIntegrationFlag(RejectionFlag::InvalidShape, IntegratorType::PixelSum);
        peak->setIntegrationFlag(RejectionFlag::InvalidShape, IntegratorType::Profile3D);
        _valid = false;
        return;
    }

    switch (_regionType) {
        case RegionType::VariableEllipsoid: {
            _peakEnd = peak_end;
            _fixed = false;
            break;
        }
        case RegionType::FixedEllipsoid: {
            // scale the ellipsoid by the average radius in pixels
            const double r = _shape.radii().sum() / 3.0;
            if (!std::isnan(r)) // Eigensolver to compute radii can fail, resulting in NaN
                _shape.scale(peak_end / r);
            else {
                peak->setIntegrationFlag(RejectionFlag::InvalidRegion, IntegratorType::PixelSum);
                peak->setIntegrationFlag(RejectionFlag::InvalidRegion, IntegratorType::Profile3D);
            }

            _peakEnd = peak_end;
            _fixed = true;
            break;
        }
        default: {
            peak->setIntegrationFlag(RejectionFlag::InvalidRegion, IntegratorType::PixelSum);
            peak->setIntegrationFlag(RejectionFlag::InvalidRegion, IntegratorType::Profile3D);
            break;
        }
    }

    if (peak->isRejectedFor(RejectionFlag::InvalidRegion)
        || peak->isRejectedFor(RejectionFlag::InterpolationFailure)
        || peak->isRejectedFor(RejectionFlag::Masked)) {
        _valid = false;
    } else {
        Ellipsoid bkg(_shape);
        bkg.scale(_bkgEnd);
        auto aabb = bkg.aabb();

        const Eigen::Vector3d& lo = aabb.lower();
        const Eigen::Vector3d& dx = aabb.upper() - aabb.lower();

        _hull.addVertex(lo);
        _hull.addVertex(lo + Eigen::Vector3d(0, 0, dx[2]));
        _hull.addVertex(lo + Eigen::Vector3d(0, dx[1], 0));
        _hull.addVertex(lo + Eigen::Vector3d(0, dx[1], dx[2]));
        _hull.addVertex(lo + Eigen::Vector3d(dx[0], 0, 0));
        _hull.addVertex(lo + Eigen::Vector3d(dx[0], 0, dx[2]));
        _hull.addVertex(lo + Eigen::Vector3d(dx[0], dx[1], 0));
        _hull.addVertex(lo + Eigen::Vector3d(dx[0], dx[1], dx[2]));
        _hull.updateHull();
    }
}

const AABB& IntegrationRegion::aabb() const
{
    return _hull.aabb();
}

AABB IntegrationRegion::peakBB() const
{
    Ellipsoid peakShape = _shape;
    if (!_fixed)
        peakShape.scale(_peakEnd);
    return peakShape.aabb();
}

void IntegrationRegion::updateMask(Eigen::MatrixXi& mask, double z) const
{
    auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (z < lower[2] || z > upper[2])
        return;

    long xmin = std::lround(std::floor(lower[0]));
    long ymin = std::lround(std::floor(lower[1]));
    long xmax = std::lround(std::ceil(upper[0]) + 1);
    long ymax = std::lround(std::ceil(upper[1]) + 1);

    xmin = std::max(0L, xmin);
    ymin = std::max(0L, ymin);

    xmax = std::min(xmax, long(mask.cols()));
    ymax = std::min(ymax, long(mask.rows()));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            EventType val = EventType(mask(y, x));
            // once forbidden, always forbidden...
            if (val == EventType::FORBIDDEN)
                continue;

            const DetectorEvent ev(x, y, z);
            auto ev_type = classify(ev);

            switch (ev_type) {
                case EventType::FORBIDDEN: val = EventType::FORBIDDEN; break;
                case EventType::PEAK: val = EventType::PEAK; break;
                case EventType::BACKGROUND:
                    if (val == EventType::EXCLUDED)
                        val = EventType::BACKGROUND;
                    break;
                default: break;
            }
            mask(y, x) = int(val);
        }
    }
}

RegionData* IntegrationRegion::getRegion()
{
    auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    long xmin = std::lround(std::floor(lower[0]));
    long ymin = std::lround(std::floor(lower[1]));
    long xmax = std::lround(std::ceil(upper[0]) + 1);
    long ymax = std::lround(std::ceil(upper[1]) + 1);
    int zmin = std::ceil(lower[2]);
    int zmax = std::floor(upper[2]);

    xmin = std::max(0L, xmin);
    ymin = std::max(0L, ymin);
    zmin = std::max(0, zmin);

    auto data = _peak->dataSet();

    xmax = std::min(xmax, long(data->nCols()));
    ymax = std::min(ymax, long(data->nRows()));
    zmax = std::min(std::size_t(zmax), data->nFrames());


    _region_data = RegionData(this, xmin, xmax, ymin, ymax, zmin, zmax);
    for (unsigned int z = zmin; z <= zmax; ++z) {
        Eigen::MatrixXi region;
        Eigen::MatrixXi mask;

        region = data->frame(z).block(ymin, xmin, ymax - ymin + 1, xmax - xmin + 1).transpose();
        // region = data->frame(z)(Eigen::seq(ymin, ymax), Eigen::seq(xmin, xmax)); // Eigen 3.4
        mask = Eigen::MatrixXi::Zero(ymax - ymin + 1, xmax - xmin + 1);

        for (auto x = xmin; x < xmax; ++x) {
            for (auto y = ymin; y < ymax; ++y) {
                EventType val = EventType::EXCLUDED;

                const DetectorEvent ev(x, y, z);
                auto ev_type = classify(ev);

                switch (ev_type) {
                    case EventType::FORBIDDEN: val = EventType::FORBIDDEN; break;
                    case EventType::PEAK: val = EventType::PEAK; break;
                    case EventType::BACKGROUND:
                        if (val == EventType::EXCLUDED)
                            val = EventType::BACKGROUND;
                        break;
                    default: break;
                }
                mask(y - ymin, x - xmin) = int(val);
            }
        }
        _region_data.addFrame(z, region, mask);
    }
    return &_region_data;
}

IntegrationRegion::EventType IntegrationRegion::classify(const DetectorEvent& ev) const
{
    Eigen::Vector3d p(ev.px, ev.py, ev.frame);

    if (_fixed) {
        Ellipsoid bb(_shape);
        Ellipsoid be(_shape);
        bb.scale(_bkgBegin);
        be.scale(_bkgEnd);

        if (_shape.isInside(p))
            return EventType::PEAK;
        else if (bb.isInside(p))
            return EventType::FORBIDDEN;
        else if (be.isInside(p))
            return EventType::BACKGROUND;
        else
            return EventType::EXCLUDED;
    } else {
        p -= _shape.center();
        const double rr = p.dot(_shape.metric() * p);
        if (rr <= _peakEnd * _peakEnd)
            return EventType::PEAK;
        if (rr > _bkgEnd * _bkgEnd)
            return EventType::EXCLUDED;
        if (rr >= _bkgBegin * _bkgBegin)
            return EventType::BACKGROUND;
        return EventType::FORBIDDEN;
    }
}

bool IntegrationRegion::advanceFrame(
    const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame,
    const Eigen::MatrixXd* gradient /* = nullptr */)
{
    const auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (frame < lower[2])
        return false;

    if (frame > upper[2])
        return true;

    const long xmin = std::max(0L, std::lround(lower[0]));
    const long ymin = std::max(0L, std::lround(lower[1]));

    const long xmax = std::min(long(image.cols()), std::lround(upper[0]));
    const long ymax = std::min(long(image.rows()), std::lround(upper[1]));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            const EventType mask_type = EventType(mask(y, x));
            if (mask_type == EventType::FORBIDDEN)
                continue;
            const DetectorEvent ev(x, y, frame);
            const Eigen::Vector3d p(x, y, frame);
            auto event_type = classify(ev);

            double grad = 0;
            if (gradient)
                grad = (*gradient)(y, x);

            if (event_type == EventType::PEAK)
                _data.addEvent(ev, image(y, x), grad);

            if (event_type == EventType::BACKGROUND && mask_type == EventType::BACKGROUND)
                _data.addEvent(ev, image(y, x), grad);

            // check if point is in Brillouin zone (or AABB if no UC available)
            // if (_hull.contains(p)) {
            //    _data.addEvent(ev, image(y,x));
            //}
        }
    }
    return false;
}

PeakData IntegrationRegion::threadSafeAdvanceFrame(
    const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame,
    const Eigen::MatrixXd* gradient /* = nullptr */)
{
    PeakData peak_data(_peak);

    const auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (frame < lower[2])
        return peak_data;

    if (frame > upper[2])
        return peak_data;

    const long xmin = std::max(0L, std::lround(lower[0]));
    const long ymin = std::max(0L, std::lround(lower[1]));

    const long xmax = std::min(long(image.cols()), std::lround(upper[0]));
    const long ymax = std::min(long(image.rows()), std::lround(upper[1]));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            const EventType mask_type = EventType(mask(y, x));
            if (mask_type == EventType::FORBIDDEN)
                continue;
            const DetectorEvent ev(x, y, frame);
            const Eigen::Vector3d p(x, y, frame);
            auto event_type = classify(ev);

            double grad = 0;
            if (gradient)
                grad = (*gradient)(y, x);

            if (event_type == EventType::PEAK)
                peak_data.addEvent(ev, image(y, x), grad);

            if (event_type == EventType::BACKGROUND && mask_type == EventType::BACKGROUND)
                peak_data.addEvent(ev, image(y, x), grad);
        }
    }
    return peak_data;
}

void IntegrationRegion::reset()
{
    _data.reset();
}

const PeakData& IntegrationRegion::peakData() const
{
    return _data;
}

PeakData& IntegrationRegion::peakData()
{
    return _data;
}

void IntegrationRegion::appendPeakData(const PeakData& peak_data)
{
    if (!peak_data.empty())
        _data.append(peak_data);
}

const Ellipsoid& IntegrationRegion::shape() const
{
    return _shape;
}

const ConvexHull& IntegrationRegion::hull() const
{
    return _hull;
}

bool IntegrationRegion::isValid() const
{
    return _valid;
}

} // namespace ohkl
