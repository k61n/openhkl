//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/IntegrationRegion.cpp
//! @brief     Implements class IntegrationRegion
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/IntegrationRegion.h"
#include "base/geometry/Ellipsoid.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

IntegrationRegion::IntegrationRegion()
{
    throw std::runtime_error("The default constructor of IntegrationRegion should no be called.");
}

IntegrationRegion::IntegrationRegion(
    Peak3D* peak, double peak_end, double bkg_begin, double bkg_end)
    : _shape(peak->shape()), _peakEnd(peak_end), _bkgBegin(bkg_begin), _bkgEnd(bkg_end), _data(peak)
{
    Ellipsoid bkg(_shape);
    bkg.scale(_bkgEnd);
    auto aabb = bkg.aabb();

    Eigen::Vector3d lo = aabb.lower();
    Eigen::Vector3d dx = aabb.upper() - aabb.lower();

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

const AABB& IntegrationRegion::aabb() const
{
    return _hull.aabb();
}

const AABB IntegrationRegion::peakBB() const
{
    Ellipsoid peakShape = _shape;
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

            DetectorEvent ev(x, y, z);
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

IntegrationRegion::EventType IntegrationRegion::classify(const DetectorEvent& ev) const
{
    Eigen::Vector3d p(ev._px, ev._py, ev._frame);
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

bool IntegrationRegion::advanceFrame(
    const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame)
{
    const auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (frame < lower[2])
        return false;

    if (frame > upper[2])
        return true;

    long xmin = std::max(0L, std::lround(lower[0]));
    long ymin = std::max(0L, std::lround(lower[1]));

    long xmax = std::min(long(image.cols()), std::lround(upper[0]));
    long ymax = std::min(long(image.rows()), std::lround(upper[1]));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            EventType mask_type = EventType(mask(y, x));
            if (mask_type == EventType::FORBIDDEN)
                continue;
            DetectorEvent ev(x, y, frame);
            Eigen::Vector3d p(x, y, frame);
            auto event_type = classify(ev);

            if (event_type == EventType::PEAK)
                _data.addEvent(ev, image(y, x));

            if (event_type == EventType::BACKGROUND && mask_type == EventType::BACKGROUND)
                _data.addEvent(ev, image(y, x));

            // check if point is in Brillouin zone (or AABB if no UC available)
            // if (_hull.contains(p)) {
            //    _data.addEvent(ev, image(y,x));
            //}
        }
    }
    return false;
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

const Ellipsoid& IntegrationRegion::shape() const
{
    return _shape;
}

const ConvexHull& IntegrationRegion::hull() const
{
    return _hull;
}

} // namespace nsx
