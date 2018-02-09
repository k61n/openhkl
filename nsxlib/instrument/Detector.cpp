/*
 nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 -----------------------------------------------------------------------------------------

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
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

#include <cmath>
#include <stdexcept>

#include "Detector.h"
#include "DetectorFactory.h"
#include "Gonio.h"

namespace nsx {

Detector* Detector::create(const YAML::Node& node)
{
    // Create an instance of the detector factory
    DetectorFactory* detectorFactory=DetectorFactory::Instance();

    // Get the detector type
    std::string detectorType = node["type"].as<std::string>();

    // Fetch the detector from the factory
    Detector* detector = detectorFactory->create(detectorType,node);

    return detector;
}

Detector::Detector()
: Component("detector"),
  _dataorder(DataOrder::BottomLeftColMajor)
{
}

Detector::Detector(const Detector& other)
: Component(other),
  _dataorder(other._dataorder)
{
}

Detector::Detector(const std::string& name)
: Component(name),
  _dataorder(DataOrder::BottomLeftColMajor)
{
}

Detector::Detector(const YAML::Node& node)
: Component(node)
{
    // If data order is not defined assumed default
    if (!node["data_ordering"]) {
        _dataorder = DataOrder::BottomRightColMajor;
        return;
    }

    std::string dataOrder=node["data_ordering"].as<std::string>();

    if (dataOrder.compare("TopLeftColMajor")==0) {
        _dataorder=DataOrder::TopLeftColMajor;
    } else if (dataOrder.compare("TopLeftRowMajor")==0) {
        _dataorder=DataOrder::TopLeftRowMajor;
    } else if (dataOrder.compare("TopRightColMajor")==0) {
        _dataorder=DataOrder::TopRightColMajor;
    } else if (dataOrder.compare("TopRightRowMajor")==0) {
        _dataorder=DataOrder::TopRightRowMajor;
    } else if (dataOrder.compare("BottomLeftColMajor")==0) {
        _dataorder=DataOrder::BottomLeftColMajor;
    } else if (dataOrder.compare("BottomLeftRowMajor")==0) {
        _dataorder=DataOrder::BottomLeftRowMajor;
    } else if (dataOrder.compare("BottomRightColMajor")==0) {
        _dataorder=DataOrder::BottomRightColMajor;
    } else if (dataOrder.compare("BottomRightRowMajor")==0) {
        _dataorder=DataOrder::BottomRightRowMajor;
    } else {
        throw std::runtime_error("Detector class: Data ordering mode not valid, can not build detector");
    }
}

Detector::~Detector()
{
}

Detector& Detector::operator=(const Detector& other)
{
    if (this != &other) {
        Component::operator=(other);
        _dataorder = other._dataorder;
    }
    return *this;
}

} // end namespace nsx

