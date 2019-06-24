//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeImage.h
//! @brief     Defines classes ImageWidget, SubframeImage
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMEIMAGE_H
#define GUI_PANELS_SUBFRAMEIMAGE_H

#include "gui/graphics/DetectorView.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

//! Part of the main window that contains the detector view
class ImageWidget : public QcrWidget {
 public:
    ImageWidget();
    void dataChanged();
    void changeView(int option);
    DetectorView* imageView;

 private:
    QcrSpinBox* frame;
    QFrame* frameLayout;
    QcrComboBox* mode;
    QSlider* slide;
    QScrollBar* scrollbar;
    QFrame* intensityLayout;
};

//! Dock widget of the main window that contains the ImageWidget
class SubframeImage : public QcrDockWidget {
 public:
    SubframeImage();
    ImageWidget* centralWidget;
};

#endif // GUI_PANELS_SUBFRAMEIMAGE_H
