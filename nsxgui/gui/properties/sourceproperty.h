//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/properties/sourceproperty.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H
#define NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class SourceProperty : public QcrWidget {
public:
    SourceProperty();

private:
    void onRemake();
    void clear();
    void onMonoChanged(int);
    void onWavelength(double);
    void onWidth(double);
    void onHeight(double);
    void onFwhm(double);
    QcrComboBox* monochromators;
    QcrComboBox* type;
    QcrDoubleSpinBox* wavelength;
    QcrDoubleSpinBox* width;
    QcrDoubleSpinBox* height;
    QcrDoubleSpinBox* fwhm;
};

#endif // NSXGUI_GUI_PROPERTIES_SOURCEPROPERTY_H
