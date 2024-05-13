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

#ifndef OHKL_CORE_MASK_IMPORTER_H
#define OHKL_CORE_MASK_IMPORTER_H

#include "base/mask/IMask.h"
#include "base/utils/YAMLType.h"

namespace ohkl {

class MaskImporter {
 public:
    //! constructs the yaml nodes from given mask data
    MaskImporter(std::string filename, int nFrames);
    //! generate Mask name from index
    std::string generateName(int number);
    //! returns restored masks
    std::vector<IMask*> getMasks();

 private:
    YAML::Node _node;
    std::vector<IMask*> _masks;
};

} // ohkl

#endif
