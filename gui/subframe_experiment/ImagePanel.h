//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeImage.h
//! @brief     Defines classes ImagePanel, SubframeImage
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_IMAGE_PANEL_H
#define GUI_SUBFRAME_EXPERIMENT_IMAGE_PANEL_H

#include "gui/graphics/DetectorView.h"

#include <QSpinBox>
#include <QComboBox>
#include <QFrame>
#include <QScrollBar>

//! Part of the main window that contains the detector view
class ImagePanel : public QWidget {

public:

   ImagePanel();
   void dataChanged();
   void changeView(int option);
   DetectorView* imageView;

private:

   QSpinBox* frame;
   QFrame* frameLayout;
   QComboBox* mode;
   QSlider* slide;
   QScrollBar* scrollbar;
   QFrame* intensityLayout;
};

// //! Dock widget of the main window that contains the ImagePanel
// class SubframeImage : public QcrDockWidget {
//  public:
//     SubframeImage();
//     ImagePanel* centralWidget;
// };

#endif // GUI_SUBFRAME_EXPERIMENT_IMAGE_PANEL_H
