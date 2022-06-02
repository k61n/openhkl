//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/properties/SampleShapeProperties.h
//! @brief     Defines class SampleShapeProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SAMPLESHAPEPROPERTIES_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SAMPLESHAPEPROPERTIES_H


#include <QLineEdit>
#include <QTableWidget>

//! Property widget for the sample and its shape
class SampleShapeProperties : public QWidget {
 public:
    SampleShapeProperties();

    void refreshInput();
    void clear();

 private:
    void facesChanged();
    void edgesChanged();
    void verticesChanged();
    void volumeChanged();

 private:
    QLineEdit* _movie;
    QLineEdit* _volume;
    QLineEdit* _faces;
    QLineEdit* _edges;
    QLineEdit* _vertices;

    // QTextTriggerButton* loadMovieButton;

    QTableWidget* sampleGoniometer;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_SAMPLESHAPEPROPERTIES_H
