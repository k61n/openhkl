//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/FlatDetector.cpp
//! @brief     Implements class FlatDetector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/detector/FlatDetector.h"

#include "base/geometry/DirectVector.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Units.h"
#include "core/detector/DetectorEvent.h"
#include "core/gonio/Gonio.h"
#include "core/gonio/RotAxis.h"
#include "core/raw/DataKeys.h"

#include <Eigen/Dense>

namespace ohkl {

Detector* FlatDetector::create(const YAML::Node& node)
{
    return new FlatDetector(node);
}

FlatDetector::FlatDetector(const YAML::Node& node) : Detector(node)
{
    // Sets the detector width from the property tree node
    auto&& widthNode = node[ym_width];
    double units = UnitsManager::get(widthNode[ym_units].as<std::string>());
    double width = widthNode[ym_value].as<double>();
    width *= units;
    setWidth(width);

    // Sets the detector height from the property tree node
    auto&& heightNode = node[ym_height];
    units = UnitsManager::get(heightNode[ym_units].as<std::string>());
    double height = heightNode[ym_value].as<double>();
    height *= units;
    setHeight(height);
}

Detector* FlatDetector::clone() const
{
    return new FlatDetector(*this);
}

void FlatDetector::setWidth(double width)
{
    _width = width;
    _angularWidth = 2.0 * atan(0.5 * _width / _distance);
}

void FlatDetector::setHeight(double height)
{
    _height = height;
    _angularHeight = 2.0 * atan(0.5 * _height / _distance);
}

void FlatDetector::setAngularWidth(double angle)
{
    _angularWidth = angle;
    _width = 2.0 * _distance * tan(angle);
}

void FlatDetector::setAngularHeight(double angle)
{
    _angularHeight = angle;
    _height = 2.0 * _distance * tan(angle);
}

DirectVector FlatDetector::pixelPosition(double px, double py) const
{
    if (_nCols == 0 || _nRows == 0)
        throw std::runtime_error("Detector: number of rows or cols must >0");

    if (_height == 0 || _width == 0)
        throw std::runtime_error("Detector: width or height must be >0");

    if (_distance == 0)
        throw std::runtime_error("Detector: distance must be >0");

    // The coordinates are defined relatively to the detector origin
    double x = px - _minCol;
    double y = py - _minRow;

    Eigen::Vector3d result;
    // take the center of the bin
    result[0] = 0.5 * _width * ((2 * x + 1.0) / _nCols - 1);
    result[1] = _distance;
    result[2] = 0.5 * _height * ((2 * y + 1.0) / _nRows - 1);
    return DirectVector(result);
}

DetectorEvent FlatDetector::constructEvent(
    const DirectVector& from, const ReciprocalVector& kf, const double frame,
    bool onDetector) const
{
    const Eigen::Vector3d direction = kf.rowVector().transpose();
    double px, py, tof;

    double x = _distance - from[1];
    if (std::fabs(direction[1]) < 1e-10 || std::fabs(x) < 1e-10)
        return {}; // no_event

    tof = x / direction[1];
    Eigen::Vector3d v = from.vector() + direction * tof;

    px = 0.5 * (_nCols * (2 * v[0] / _width + 1) - 1);
    py = 0.5 * (_nRows * (2 * v[2] / _height + 1) - 1);

    if (px < 0 || px > _nCols || py < 0 || py > _nRows) {
        if (onDetector)
            return {}; // no event
    }

    return {px, py, frame, tof};
}

Eigen::Matrix3d FlatDetector::jacobian(double /*px*/, double /*py*/) const
{
    Eigen::Matrix3d J;
    J.setZero();

    if (_nCols == 0 || _nRows == 0)
        throw std::runtime_error("Detector: number of rows or cols must >0");

    if (_height == 0 || _width == 0)
        throw std::runtime_error("Detector: width or height must be >0");

    if (_distance == 0)
        throw std::runtime_error("Detector: distance must be >0");

    J(0, 0) = _width / _nCols;
    J(2, 1) = _height / _nRows;

    return J;
}

} // namespace ohkl
