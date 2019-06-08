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


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_IMAGE_H
#define NSXGUI_GUI_PANELS_SUBFRAME_IMAGE_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QGraphicsView>

//! Subframe of the main window, with tabs to control detector, fits, &c

class ImageWidget : public QcrWidget {
public:
    ImageWidget();

private:
    QcrSpinBox* max;
    QcrSpinBox* frame;
    QcrComboBox* mode;
    QSlider* slide;
    QScrollBar* scrollbar;
    QGraphicsView* imageView;
    QGraphicsScene* imageScene;
};

class SubframeImage : public QcrDockWidget {
public:
    SubframeImage();

private:
    ImageWidget* centralWidget;
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_IMAGE_H
