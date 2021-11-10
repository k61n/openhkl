//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subwindows/PeakWindow.cpp
//! @brief     Implements class PeakWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_PEAKWINDOW_H
#define NSX_GUI_SUBWINDOWS_PEAKWINDOW_H

#include "core/peak/RegionData.h"

#include <Eigen/Core>

#include <QWidget>
#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSlider>

class ColorMap;

namespace nsx {
class IntegrationRegion;
}

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class PeakWindow : public QDialog {
 public:
    PeakWindow(QWidget* parent = nullptr, nsx::IntegrationRegion* region = nullptr);

    //! set the integration region
    void setIntegrationRegion(nsx::IntegrationRegion* region);
    //! Refresh the whole dialog
    void refreshAll();
    //! Set the frame index
    void setFrame(std::size_t frame_index);
    //! Plot the integration region image with overlay
    void drawFrame();

 private:
    void setGraphicsViewUp();
    QImage* getIntegrationMask(const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg);

    nsx::IntegrationRegion* _integration_region;
    nsx::RegionData _region_data;

    unsigned int _frame_index;
    int _intensity;
    bool _logarithmic;

    QWidget* _preview_widget;
    QWidget* _control_widget;

    ColorMap* _colormap;
    QGraphicsView* _graphics_view;
    QGraphicsPixmapItem* _image;
    QGraphicsPixmapItem* _integration_overlay;
    QSlider* _frame_slider;

    QColor _peak_color;
    QColor _bkg_color;
};

#endif // NSX_GUI_SUBWINDOWS_PEAKWINDOW_H
