//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/properties/detectorproperty.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableWidget>

class DetectorProperty : public QcrWidget {
public:
    DetectorProperty();
    ~DetectorProperty();

private:
    void onValueChanged();
    void onRemake();

    QcrSpinBox* columns;
    QcrSpinBox* rows;
    QcrDoubleSpinBox* height;
    QcrDoubleSpinBox* width;
    QcrDoubleSpinBox* distance;
    QTableWidget* axes;
};

#endif // NSXGUI_GUI_PROPERTIES_DETECTORPROPERTY_H
