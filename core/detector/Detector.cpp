//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/Detector.cpp
//! @brief     Implements class Detector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <stdexcept>

#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Detector* Detector::create(const YAML::Node& node)
{
    std::string detectorType = node[nsx::ym_detectorType].as<std::string>();

    return DetectorFactory::instance().create(detectorType, node);
}

Detector::Detector()
    : Component(nsx::kw_detectorName0)
    , _height(0.0)
    , _width(0.0)
    , _angularHeight(0.0)
    , _angularWidth(0.0)
    , _nRows(0)
    , _nCols(0)
    , _minRow(0.0)
    , _minCol(0.0)
    , _distance(0)
    , _dataorder(DataOrder::BottomLeftColMajor)
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
    , _dataorder(DataOrder::BottomLeftColMajor)
    , _baseline(0.0)
    , _gain(1.0)
{
}

Detector::Detector(const YAML::Node& node) : Component(node)
{
    // If data order is not defined assumed default
    if (!node[nsx::ym_dataOrdering]) {
        _dataorder = DataOrder::BottomRightColMajor;
        return;
    }

    // detector gain
    if (node[nsx::ym_gain])
        _gain = node[nsx::ym_gain].as<double>();
    else
        _gain = 1.0;

    // detector baseline
    if (node[nsx::ym_baseline])
        _baseline = node[nsx::ym_baseline].as<double>();
    else
        _baseline = 0.0;

    std::string dataOrder = node[nsx::ym_dataOrdering].as<std::string>();

    if (dataOrder.compare("TopLeftColMajor") == 0)
        _dataorder = DataOrder::TopLeftColMajor;
    else if (dataOrder.compare("TopLeftRowMajor") == 0)
        _dataorder = DataOrder::TopLeftRowMajor;
    else if (dataOrder.compare("TopRightColMajor") == 0)
        _dataorder = DataOrder::TopRightColMajor;
    else if (dataOrder.compare("TopRightRowMajor") == 0)
        _dataorder = DataOrder::TopRightRowMajor;
    else if (dataOrder.compare("BottomLeftColMajor") == 0)
        _dataorder = DataOrder::BottomLeftColMajor;
    else if (dataOrder.compare("BottomLeftRowMajor") == 0)
        _dataorder = DataOrder::BottomLeftRowMajor;
    else if (dataOrder.compare("BottomRightColMajor") == 0)
        _dataorder = DataOrder::BottomRightColMajor;
    else if (dataOrder.compare("BottomRightRowMajor") == 0)
        _dataorder = DataOrder::BottomRightRowMajor;
    else {
        throw std::runtime_error(
            "Detector class: Data ordering mode not valid, can not build detector");
    }

    // Sets the detector to sample distance from the property tree node
    auto&& distanceNode = node[nsx::ym_sampleDistance];
    double units = UnitsManager::get(distanceNode[nsx::ym_units].as<std::string>());
    double distance = distanceNode[nsx::ym_value].as<double>();
    distance *= units;
    setDistance(distance);

    // Sets the detector number of pixels from the property tree node
    unsigned int nCols = node[nsx::ym_colsNr].as<unsigned int>();
    setNCols(nCols);

    unsigned int nRows = node[nsx::ym_rowsNr].as<unsigned int>();
    setNRows(nRows);

    _minCol = node[nsx::ym_originX] ? node[nsx::ym_originX].as<double>() : 0.0;
    _minRow = node[nsx::ym_originY] ? node[nsx::ym_originY].as<double>() : 0.0;
}

Detector::~Detector() = default;

DataOrder Detector::dataOrder() const
{
    return _dataorder;
}

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

} // namespace nsx
