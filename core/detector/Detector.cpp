//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/Detector.cpp
//! @brief     Implements class Detector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <stdexcept>

#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

Detector* Detector::create(const YAML::Node& node)
{
    std::string detectorType = node[ohkl::ym_detectorType].as<std::string>();

    return DetectorFactory::instance().create(detectorType, node);
}

Detector::Detector()
    : Component(ohkl::kw_detectorDefaultName)
    , _height(0.0)
    , _width(0.0)
    , _angularHeight(0.0)
    , _angularWidth(0.0)
    , _nRows(0)
    , _nCols(0)
    , _minRow(0.0)
    , _minCol(0.0)
    , _distance(0)
    , _baseline(0.0)
    , _gain(1.0)
{
}

Detector::Detector(const std::string& name)
    : Component(name)
    , _height(0.0)
    , _width(0.0)
    , _angularHeight(0.0)
    , _angularWidth(0.0)
    , _nRows(0)
    , _nCols(0)
    , _minRow(0.0)
    , _minCol(0.0)
    , _distance(0)
    , _baseline(0.0)
    , _gain(1.0)
{
}

Detector::Detector(const YAML::Node& node) : Component(node)
{
    // detector gain
    if (node[ohkl::ym_gain])
        _gain = node[ohkl::ym_gain].as<double>();
    else
        _gain = 1.0;

    // detector baseline
    if (node[ohkl::ym_baseline])
        _baseline = node[ohkl::ym_baseline].as<double>();
    else
        _baseline = 0.0;

    // Sets the detector to sample distance from the property tree node
    auto&& distanceNode = node[ohkl::ym_sampleDistance];
    double units = UnitsManager::get(distanceNode[ohkl::ym_units].as<std::string>());
    double distance = distanceNode[ohkl::ym_value].as<double>();
    distance *= units;
    setDistance(distance);

    // Sets the detector number of pixels from the property tree node
    // unsigned int nCols = node[ohkl::ym_colCount].as<unsigned int>();
    // Eigen::Vector3d axis = node[ohkl::ym_axisDirection].as<Eigen::Vector3d>();

    // supporting multiple resolution for one detector (up to three)
    // entry of 0 is considered to be invalid/no entry
    auto cols = node[ohkl::ym_colCount].as<Eigen::Vector3d>();
    auto rows = node[ohkl::ym_rowCount].as<Eigen::Vector3d>();

    for (int i = 0; i < 3; i++)
        if (cols[i] > 0 && rows[i] > 0) {
            _nCols_options.emplace_back(cols[i]);
            _nRows_options.emplace_back(rows[i]);
        }

    if (_nCols_options.size() == 0) // if no non zero resolutions are found
        throw std::runtime_error("Detector::Detector Found no valid non zero resolution "
                                 "definition for detector in yaml file: ");

    // set first resolution as default - user can change this during file importing
    setNCols(_nCols_options[0]);
    setNRows(_nRows_options[0]);

    _minCol = node[ohkl::ym_originX] ? node[ohkl::ym_originX].as<double>() : 0.0;
    _minRow = node[ohkl::ym_originY] ? node[ohkl::ym_originY].as<double>() : 0.0;
}

Detector::~Detector() = default;


double Detector::baseline() const
{
    return _baseline;
}

double Detector::gain() const
{
    return _gain;
}

double Detector::height() const
{
    return _height;
}

double Detector::width() const
{
    return _width;
}

int Detector::minRow() const
{
    return _minRow;
}

int Detector::maxRow() const
{
    return _minRow + _nRows;
}

int Detector::minCol() const
{
    return _minCol;
}

int Detector::maxCol() const
{
    return _minCol + _nCols;
}

double Detector::angularHeight() const
{
    return _angularHeight;
}

double Detector::angularWidth() const
{
    return _angularWidth;
}

double Detector::distance() const
{
    return _distance;
}

void Detector::setDistance(double d)
{
    _distance = d;
}

bool Detector::hasPixel(double px, double py) const
{
    double dx = px - _minCol;
    double dy = py - _minRow;

    return (
        dx >= 0 && dx < static_cast<double>(_nCols) && dy >= 0 && dy < static_cast<double>(_nRows));
}

unsigned int Detector::nCols() const
{
    return _nCols;
}

void Detector::setNCols(unsigned int cols)
{
    if (cols == 0)
        throw std::range_error(
            "Detector '" + Component::_name + "' number of pixels (row,col) must be >0");
    _nCols = cols;
}

unsigned int Detector::nRows() const
{
    return _nRows;
}

void Detector::setNRows(unsigned int rows)
{
    if (rows == 0)
        throw std::range_error(
            "Detector '" + Component::_name + "' number of pixels (row,col) must be >0");
    _nRows = rows;
}

double Detector::pixelHeight() const
{
    return _height / _nRows;
}

double Detector::pixelWidth() const
{
    return _width / _nCols;
}

void Detector::setBaseline(double baseline)
{
    _baseline = baseline;
}

void Detector::setGain(double gain)
{
    _gain = gain;
}

} // namespace ohkl
