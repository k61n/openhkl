//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_monochromaticsource.h
//! @brief     Defines class SubtabMonochromatic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
#define GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H

#include "gui/properties/sourceproperty.h"

class SubtabMonochromatic : public QcrWidget {
public:
    SubtabMonochromatic();

private:
    SourceProperty* property;
};

#endif // GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
