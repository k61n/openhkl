//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_monochromaticsource.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
#define NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H

#include "gui/properties/sourceproperty.h"
#include <QCR/widgets/views.h>

class SubtabMonochromatic : public QcrWidget {
public:
    SubtabMonochromatic();

private:
    SourceProperty* property;
};

#endif // NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
