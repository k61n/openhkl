//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/MaskImporter
//! @brief     Defines class MaskImporter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MaskImporter.h"
#include <fstream>

namespace ohkl {
MaskImporter::MaskImporter(std::string filename)
{
    _node = YAML::LoadFile(filename.c_str());
    int size = _node["size"].as<int>();

    for (int i = 0; i < size; ++i) {
        std::string name = generateName(i);
        std::string type = _node[name]["Type"].as<std::string>();

        IMask* mask;
        Eigen::Vector3d upper;
        Eigen::Vector3d lower;

        upper[0] = _node[name]["UpperX"].as<float>();
        upper[1] = _node[name]["UpperY"].as<float>();
        upper[2] = 150;

        lower[0] = _node[name]["LowerX"].as<float>();
        lower[1] = _node[name]["LowerY"].as<float>();
        lower[2] = 0;

        AABB aabb(upper, lower);

        if (type == "Rectangle")
            mask = new BoxMask(aabb);
        else if (type == "Ellipse")
            mask = new EllipseMask(aabb);
        else
            throw std::runtime_error("E MaskImporter::MaskImporter Invalid Mask type found");

        _masks.emplace_back(mask);
    }
}

std::string MaskImporter::generateName(int number)
{
    int nDigits = 2;
    std::string str = std::to_string(number + 1);
    return std::string("Mask") + std::string(nDigits - str.size(), '0').append(str);
}

std::vector<IMask*> MaskImporter::getMasks()
{
    return _masks;
}
} // ohkl