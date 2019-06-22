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

namespace nsx {

Detector* Detector::create(const YAML::Node& node)
{
    // Create an instance of the detector factory
    DetectorFactory* detectorFactory = DetectorFactory::Instance();

    // Gets the detector type
    std::string detectorType = node["type"].as<std::string>();

    // Fetch the detector from the factory
    Detector* detector = detectorFactory->create(detectorType, node);

    return detector;
}

Detector::Detector()
    : Component("detector")
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
    if (!node["data_ordering"]) {
        _dataorder = DataOrder::BottomRightColMajor;
        return;
    }

    // detector gain
    if (node["gain"])
        _gain = node["gain"].as<double>();
    else
        _gain = 1.0;

    // detector baseline
    if (node["baseline"])
        _baseline = node["baseline"].as<double>();
    else
        _baseline = 0.0;

    std::string dataOrder = node["data_ordering"].as<std::string>();

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

    UnitsManager* um = UnitsManager::Instance();

    // Sets the detector to sample distance from the property tree node
    auto&& distanceNode = node["sample_distance"];
    double units = um->get(distanceNode["units"].as<std::string>());
    double distance = distanceNode["value"].as<double>();
    distance *= units;
    setDistance(distance);

    // Sets the detector number of pixels from the property tree node
    unsigned int nCols = node["ncols"].as<unsigned int>();
    setNCols(nCols);

    unsigned int nRows = node["nrows"].as<unsigned int>();
    setNRows(nRows);

    _minCol = node["origin_x"] ? node["origin_x"].as<double>() : 0.0;
    _minRow = node["origin_y"] ? node["origin_y"].as<double>() : 0.0;
}

Detector::~Detector() {}

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
            "Detector " + Component::_name + " number of pixels (row,col) must be >0");
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
            "Detector " + Component::_name + " number of pixels (row,col) must be >0");
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
