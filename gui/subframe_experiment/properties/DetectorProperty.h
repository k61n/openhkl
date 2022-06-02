//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/properties/DetectorProperty.h
//! @brief     Defines class DetectorProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_DETECTORPROPERTY_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_DETECTORPROPERTY_H

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTableWidget>

//! Property widget of the detector
class DetectorProperty : public QWidget {
 public:
    DetectorProperty();
    ~DetectorProperty();
    void refreshInput();

 private:
    void onValueChanged();

    QSpinBox* _columns;
    QSpinBox* _rows;
    QDoubleSpinBox* _height;
    QDoubleSpinBox* _width;
    QDoubleSpinBox* _distance;
    QTableWidget* _axes;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_DETECTORPROPERTY_H
