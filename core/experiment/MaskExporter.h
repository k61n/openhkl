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

#ifndef OHKL_CORE_MASK_EXPORTER_H
#define OHKL_CORE_MASK_EXPORTER_H

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/mask/IMask.h"
#include "base/utils/YAMLType.h"

#include <set>
#include <string>

namespace ohkl {
class MaskExporter {
 public:
    //! constructs the yaml nodes from given mask data
    MaskExporter(std::vector<ohkl::IMask*> data);
    //! export to yaml file
    void exportToFile(std::string filename);
    //! generate mask name from index
    std::string generateName(int number);

 private:
    YAML::Node _node;
};

} // ohkl

#endif