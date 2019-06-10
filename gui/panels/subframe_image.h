//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_image.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAME_IMAGE_H
#define GUI_PANELS_SUBFRAME_IMAGE_H

#include "gui/graphics/detectorview.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

//! Subframe of the main window, with tabs to control detector, fits, &c

class ImageWidget : public QcrWidget {
public:
    ImageWidget();
    void dataChanged();
    void changeView(int option);
    DetectorView* imageView;

private:
    QcrSpinBox* max;
    QcrSpinBox* frame;
    QFrame* frameLayout;
    QcrComboBox* mode;
    QSlider* slide;
    QScrollBar* scrollbar;
    QFrame* intensityLayout;
};

class SubframeImage : public QcrDockWidget {
public:
    SubframeImage();
    ImageWidget* centralWidget;
};

#endif // GUI_PANELS_SUBFRAME_IMAGE_H
