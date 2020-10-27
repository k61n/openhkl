//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/ImagePanel.h
//! @brief     Defines classes ImagePanel, SubframeImage
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_IMAGEPANEL_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_IMAGEPANEL_H

#include <QComboBox>
#include <QFrame>
#include <QScrollBar>
#include <QSpinBox>

class DetectorView;

//! Part of the main window that contains the detector view
class ImagePanel : public QWidget {
 public:
    ImagePanel();
    void dataChanged(int numor = 0);
    void changeView(int option);

    DetectorView* getView() { return _image_view; };

    QSlider* getMaxIntensitySlider() { return _slider; }

 private:
    DetectorView* _image_view;

    QSpinBox* _frame;
    QComboBox* _mode;
    QSlider* _slider;
    QScrollBar* _scrollbar;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_IMAGEPANEL_H
