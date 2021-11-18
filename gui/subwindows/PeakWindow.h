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

#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QSlider>

class ColorButton;
class ColorMap;

namespace nsx {
class IntegrationRegion;
}

struct PeakWindowParameters {
    double peak_end = 3.0;
    double bkg_begin = 3.0;
    double bkg_end = 6.0;
    QColor peak_color = Qt::green;
    QColor bkg_color = Qt::yellow;
    double alpha = 0.2;
    int max_intensity = 3000;
};

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class PeakWindow : public QDialog {

 public:
    PeakWindow(QWidget* parent = nullptr, nsx::IntegrationRegion* region = nullptr);

    //! set the integration region
    void setIntegrationRegion(nsx::IntegrationRegion* region);
    //! Refresh the whole dialog
    void refreshAll();

    QSize sizeHint() const;

 private:
    //! set up control widget
    void setControlWidgetUp();
    //! Plot the integration region image with overlay
    QGraphicsView* drawFrame(std::size_t frame_index);
    //! Convert matrix to integration mask QImage
    QImage* getIntegrationMask(const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg);
    //! Set the peak description
    void setLabel();
    //! Remove this peak from the global vector
    void remove();

    void grabParameters();
    void setParameters();

    void closeEvent(QCloseEvent* event) override;

    nsx::IntegrationRegion* _integration_region;
    nsx::RegionData* _region_data;

    int _intensity;
    bool _logarithmic;

    QWidget* _scroll_widget;

    ColorMap* _colormap;
    QVector<QGraphicsView*> _views;
    QGridLayout* _grid_layout;
    QGraphicsView* _graphics_view;
    QGridLayout* _control_layout;

    QDoubleSpinBox* _peak_end;
    QDoubleSpinBox* _bkg_begin;
    QDoubleSpinBox* _bkg_end;
    QDoubleSpinBox* _alpha;
    ColorButton* _peak_color_button;
    ColorButton* _bkg_color_button;
    QSlider* _intensity_slider;

    QColor _peak_color;
    QColor _bkg_color;

    static PeakWindowParameters _params;
};

#endif // NSX_GUI_SUBWINDOWS_PEAKWINDOW_H
