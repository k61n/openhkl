//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/resources/D19.cpp from core/resources/.cpp.in
//! @brief     Defines an instrument specific resource function
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "D19.h"

namespace nsx {

const std::vector<std::string>& resource_instruments_D19()
{
    static std::vector<std::string> data = {
        {"---"},
        {"instrument:"},
        {"    name: D19"},
        {"    detector:"},
        {"        type: cylindrical"},
        {"        name: banana"},
        {"        sample_distance:"},
        {"            value: 764.0"},
        {"            units: mm"},
        {"        angular_width:"},
        {"            value: 120.0"},
        {"            units: deg"},
        {"        height:"},
        {"            units: m"},
        {"            value: 0.4"},
        {"        nrows: 256"},
        {"        ncols: 640"},
        {"        row_min: 0"},
        {"        col_min: 0"},
        {"        data_ordering: TopRightColMajor"},
        {"        gain: 0.86"},
        {"        baseline: 0.0"},
        {"        goniometer:"},
        {"            name: detector-gonio"},
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
        {"                  direction: [1.0,0.0,0.0]"},
        {"                  clockwise: true"},
        {"                  physical: false"},
        {"    sample:"},
        {"        name: sample"},
        {"        goniometer:"},
        {"            name: sample-gonio"},
        {"            axis:"},
        {"                - name: omega"},
        {"                  type: rotation"},
        {"                  id: 2"},
        {"                  direction: [0.0,0.0,1.0]"},
        {"                  clockwise: true"},
        {"                  physical: true"},
        {""},
        {"                - name: chi"},
        {"                  type: rotation"},
        {"                  id: 3"},
        {"                  direction: [0.0,1.0,0.0]"},
        {"                  clockwise: false"},
        {"                  physical: true"},
        {""},
        {"                - name: phi"},
        {"                  type: rotation"},
        {"                  id: 4"},
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
