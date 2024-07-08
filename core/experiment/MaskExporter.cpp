//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/mask/MaskExporter
//! @brief     Defines class MaskExporter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MaskExporter.h"

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"

#include <fstream>

namespace ohkl {
MaskExporter::MaskExporter(std::vector<ohkl::IMask*> masks)
{
    int idx = 0;
    _node["size"] = idx;

    for (auto& e : masks) {
        YAML::Node mask;
        std::string name = generateName(idx);

        if (dynamic_cast<const ohkl::BoxMask*>(e) != nullptr) {
            mask["Type"] = "Rectangle";
        } else if (dynamic_cast<const ohkl::EllipseMask*>(e) != nullptr) {
            mask["Type"] = "Ellipse";
        } else
            throw std::runtime_error("MaskExporter::addMasks Invalid mask type");

        mask["LowerX"] = std::round(e->aabb().lower()[0] * 100) / 100;
        mask["LowerY"] = std::round(e->aabb().lower()[1] * 100) / 100;
        mask["UpperX"] = std::round(e->aabb().upper()[0] * 100) / 100;
        mask["UpperY"] = std::round(e->aabb().upper()[1] * 100) / 100;

        _node[name] = mask;
        ++idx;
    }
    _node["size"] = idx;
}

std::string MaskExporter::generateName(int number)
{
    int nDigits = 2;
    std::string str = std::to_string(number + 1);
    return std::string("Mask ") + std::string(nDigits - str.size(), '0').append(str);
}

void MaskExporter::exportToFile(std::string filename)
{
    std::fstream fout(filename.c_str(), std::ios::out);
    fout << _node;
    fout.close();
}

} // namespace ohkl
