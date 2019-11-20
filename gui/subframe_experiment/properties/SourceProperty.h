//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/SourceProperty.h
//! @brief     Defines class SourceProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SOURCEPROPERTY_H
#define GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SOURCEPROPERTY_H

#include <QComboBox>
#include <QDoubleSpinBox>

//! Property widget of the monochromatic source
class SourceProperty : public QWidget {
 public:

    SourceProperty();
    void refreshInput();
    void clear();

 private:
 
    void onMonoChanged(int);
    void onWavelength(double);
    void onWidth(double);
    void onHeight(double);
    void onFwhm(double);

    QComboBox* _monochromators;
    QComboBox* _type;
    QDoubleSpinBox* _wavelength;
    QDoubleSpinBox* _width;
    QDoubleSpinBox* _height;
    QDoubleSpinBox* _FWHM;

};

#endif // GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SOURCEPROPERTY_H
