//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/resources/D9_large_lifting_arm.cpp from core/resources/.cpp.in
//! @brief     Defines an instrument specific resource function
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "D9_large_lifting_arm.h"

namespace nsx {

const std::vector<std::string>& resource_instruments_D9_large_lifting_arm()
{
    static std::vector<std::string> data = {
        {"---"},
        {"instrument:"},
        {"    name: D9"},
        {"    detector:"},
        {"        name: banana"},
        {"        type: flat"},
        {"        sample_distance:"},
        {"            value: 368.0"},
        {"            units: mm"},
        {"        width:"},
        {"            value: 256.0"},
        {"            units: mm"},
        {"        height:"},
        {"           value: 256.0"},
        {"           units: mm"},
        {"        nrows: 128"},
        {"        ncols: 128"},
        {"        row_min: 0"},
        {"        col_min: 0"},
        {"        data_ordering: BottomRightColMajor"},
        {"        goniometer:"},
        {"            name: detector-lifting-arm"},
        {"            axis:"},
        {"                - name: 2theta(gamma)"},
        {"                  type: rotation"},
        {"                  id: 1"},
        {"                  direction: [0.0,0.0,1.0]"},
        {"                  clockwise: true"},
        {"                  physical: true"},
        {""},
        {"                - name: 2theta(nu)"},
        {"                  type: rotation"},
        {"                  id: 3"},
        {"                  direction: [1.0,0.0,0.0]"},
        {"                  clockwise: false"},
        {"                  physical: true"},
        {"    sample:"},
        {"        name: sample=gonio"},
        {"        goniometer:"},
        {"            name: Euler cradle (BL)"},
        {"            axis:"},
        {"                - name: omega"},
        {"                  type: rotation"},
        {"                  id: 16"},
        {"                  direction: [0.0,0.0,1.0]"},
        {"                  clockwise: true"},
        {"                  physical: true"},
        {"    source:"},
        {"        name: monochromatic source"},
        {"        monochromator:"},
        {"            name: mono"},
        {"            width:"},
        {"                value: 1.0"},
        {"                units: mm"},
        {"            height:"},
        {"                value: 1.0"},
        {"                units: mm"},
        {"            wavelength:"},
        {"                value: 1.24"},
        {"                units: ang"},
        {"            fwhm:"},
        {"                value: 0.00901"},
        {"                units: ang"},
        {"..."},
    };
    return data;
}

} // namespace nsx
