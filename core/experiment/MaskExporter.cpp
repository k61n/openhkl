//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/mask/MaskExporter
//! @brief     Defines class MaskExporter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MaskExporter.h"
#include <fstream>

namespace ohkl{
MaskExporter::MaskExporter(std::set<ohkl::IMask*> masks)
{
    int idx = 0;
    _node["size"] = idx;

    for (auto & e : masks){
        YAML::Node mask;
        std::string name = generateName(idx);

        if (dynamic_cast<const ohkl::BoxMask*>(e) != nullptr) {
            mask["Type"] = "Rectangle";
        } else if (dynamic_cast<const ohkl::EllipseMask*>(e) != nullptr) {
            mask["Type"] = "Ellipse";
        } else
            throw std::runtime_error("E MaskExporter::addMasks Invalid mask type found! ");

            mask["LowerX"] = std::round(e->aabb().lower()[0] * 100)/100;
            mask["LowerY"] = std::round(e->aabb().lower()[1] * 100)/100;
            mask["UpperX"] = std::round(e->aabb().upper()[0] * 100)/100;
            mask["UpperY"] = std::round(e->aabb().upper()[1] * 100)/100;

            //mask["UpperFrame"]  = (e->aabb().upper()[2]); // 150
            //mask["LowerFrame"] = (e->aabb().lower()[2]);  // 0
      
        _node[name] = mask;
        ++idx; 
    }
    _node["size"] = idx;
}

std::string MaskExporter::generateName(int number)
{
    int n = 2; // number of digits
    std::string str = std::to_string(number + 1);
    return std::string("Mask") + std::string(n - str.size(), '0').append(str);
}

void MaskExporter::exportToFile(std::string filename) 
{
    std::fstream fout(filename.c_str(), std::ios::out);
    fout << _node; 
    fout.close();
}
}//ohkl