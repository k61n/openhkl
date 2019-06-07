//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/properties/sampleshapeproperties.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H
#define NSXGUI_GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H

#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableWidget>

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

#endif // NSXGUI_GUI_PROPERTIES_SAMPLESHAPEPROPERTIES_H
