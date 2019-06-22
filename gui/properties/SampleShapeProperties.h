//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/SampleShapeProperties.h
//! @brief     Defines class SampleShapeProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H
#define GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H

#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableWidget>

//! Property widget for the sample and its shape
class SampleShapeProperties : public QcrWidget {
 public:
    SampleShapeProperties();

 private:
    void onRemake();
    void clear();
    void facesChanged();
    void edgesChanged();
    void verticesChanged();
    void volumeChanged();
    QcrLineEdit* movie;
    QcrLineEdit* volume;
    QcrLineEdit* faces;
    QcrLineEdit* edges;
    QcrLineEdit* vertices;
    QcrTextTriggerButton* loadMovieButton;
    QTableWidget* sampleGoniometer;
};

#endif // GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H
