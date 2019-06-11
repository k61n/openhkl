//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/resources/BioDiff2500.cpp from core/resources/.cpp.in
//! @brief     Defines an instrument specific resource function
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "BioDiff2500.h"

namespace nsx {

const std::vector<std::string>& resource_instruments_BioDiff2500()
{
    static std::vector<std::string> data = {
        {"---"},
        {"instrument:"},
        {"    name: BioDiff2500"},
        {"    detector:"},
        {"        type: cylindrical"},
        {"        name: banana"},
        {"        sample_distance:"},
        {"            value: 199.0"},
        {"            units: mm"},
        {"        angular_width:"},
        {"            value: 360.0"},
        {"            units: deg"},
        {"        height:"},
        {"            value: 0.44824"},
        {"            units: m"},
        {"        row_min: 0"},
        {"        col_min: 0"},
        {"        nrows: 900"},
        {"        ncols: 2500"},
        {"        data_ordering: TopLeftRowMajor"},
        {"        gain: 7.0"},
        {"        baseline: 227.0"},
        {"        goniometer:"},
        {"            name: detector-gonio"},
        {"            axis:"},
        {"                - name: 2theta(gamma)"},
        {"                  type: rotation"},
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
        {"                  direction: [0.0,0.0,1.0]"},
        {"                  clockwise: true"},
        {"                  physical: true"},
        {""},
        {"                - name: chi"},
        {"                  type: rotation"},
        {"                  direction: [0.0,1.0,0.0]"},
        {"                  clockwise: false"},
        {"                  physical: true"},
        {""},
        {"                - name: phi"},
        {"                  type: rotation"},
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
        {"                value: 2.6734"},
        {"                units: ang"},
        {"            fwhm:"},
        {"                value: 0.00901"},
        {"                units: ang"},
        {"..."},
    };
    return data;
}

} // namespace nsx
