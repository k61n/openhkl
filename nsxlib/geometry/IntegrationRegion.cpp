/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "BrillouinZone.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Ellipsoid.h"
#include "IntegrationRegion.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

IntegrationRegion::IntegrationRegion()
{
    throw std::runtime_error("The default constructor of IntegrationRegion should no be called.");
}

IntegrationRegion::IntegrationRegion(sptrPeak3D peak, double peak_end, double bkg_begin, double bkg_end):
    _shape(peak->getShape()),
    _peakEnd(peak_end),
    _bkgBegin(bkg_begin),
    _bkgEnd(bkg_end),
    _data(peak)
{
    auto uc = peak->activeUnitCell();

    // try to find Brillouin zone if peak has been indexed
    if (uc && peak->isIndexed()) {
        BrillouinZone zone(uc->reciprocalBasis());
        _hull = zone.detectorConvexHull(peak->q(), peak->data());
    }
    // otherwise, just use bounding box of the integration ellipsoid
    else {
        Ellipsoid bkg(_shape);
        bkg.scale(_bkgEnd);
        auto aabb = bkg.aabb();

        Eigen::Vector3d lo = aabb.lower();
        Eigen::Vector3d dx = aabb.upper() - aabb.lower();

        _hull.addVertex(lo);
        _hull.addVertex(lo+Eigen::Vector3d(0, 0, dx[2]));
        _hull.addVertex(lo+Eigen::Vector3d(0, dx[1], 0));
        _hull.addVertex(lo+Eigen::Vector3d(0, dx[1], dx[2]));
        _hull.addVertex(lo+Eigen::Vector3d(dx[0], 0, 0));
        _hull.addVertex(lo+Eigen::Vector3d(dx[0], 0, dx[2]));
        _hull.addVertex(lo+Eigen::Vector3d(dx[0], dx[1], 0));
        _hull.addVertex(lo+Eigen::Vector3d(dx[0], dx[1], dx[2]));
    }
    _hull.updateHull();
}

const AABB IntegrationRegion::aabb() const
{
    return _hull.aabb();
}

void IntegrationRegion::updateMask(Eigen::MatrixXi& mask, double z) const
{
    auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (z < lower[2] || z > upper[2]) {
        return;
    }

    long xmin = std::lround(std::floor(lower[0]));
    long ymin = std::lround(std::floor(lower[1]));
    long xmax = std::lround(std::ceil(upper[0])+1);
    long ymax = std::lround(std::ceil(upper[1])+1);

    xmin = std::max(0l, xmin);
    ymin = std::max(0l, ymin);

    xmax = std::min(xmax, long(mask.cols()));
    ymax = std::min(ymax, long(mask.rows()));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {    
            // once forbidden, always forbidden...
            if (mask(y,x) == int(EventType::FORBIDDEN)) {
                continue;
            }

            DetectorEvent ev(x, y, z);
            auto s = classify(ev);
            
            if (s == EventType::FORBIDDEN) {
                mask(y, x) = int(s);
                continue;
            }

            if (s == EventType::PEAK) {
                mask(y, x) = int(s);
                continue;
            }

            if (s == EventType::BACKGROUND && mask(y,x) == int(EventType::EXCLUDED)) {
                mask(y, x) = int(s);
                continue;
            }
        }
    }
}

IntegrationRegion::EventType IntegrationRegion::classify(const DetectorEvent& ev) const
{
    Eigen::Vector3d p(ev._px, ev._py, ev._frame);
    p -= _shape.center();
    const double rr = p.dot(_shape.metric()*p);

    if (rr <= _peakEnd*_peakEnd) {
        return EventType::PEAK;
    }
    if (rr > _bkgEnd*_bkgEnd) {
        return EventType::EXCLUDED;
    }
    if (rr >= _bkgBegin*_bkgBegin) {
        return EventType::BACKGROUND;
    }
    return EventType::FORBIDDEN;
}

bool IntegrationRegion::advanceFrame(const Eigen::MatrixXi& image, const Eigen::MatrixXi& mask, double frame)
{
    const auto aabb = _hull.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (frame < lower[2]) {
        return false;
    }

    if (frame > upper[2]) {
        return true;
    }

    long xmin = std::max(0L, std::lround(lower[0]));
    long ymin = std::max(0L, std::lround(lower[1]));

    long xmax = std::min(long(image.cols()), std::lround(upper[0]));
    long ymax = std::min(long(image.rows()), std::lround(upper[0]));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            DetectorEvent ev(x, y, frame);
            Eigen::Vector3d p(x, y, frame);
            // check if point is in Brillouin zone (or AABB if no UC available)
            if (_hull.contains(p)) {
                _data.addEvent(ev, image(y,x));
            }          
        }
    }
    return false;
}

void IntegrationRegion::reset()
{
    _data.reset();
}

const PeakData& IntegrationRegion::data() const
{
    return _data;
}

PeakData& IntegrationRegion::data()
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

} // end namespace nsx
