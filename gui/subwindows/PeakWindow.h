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

#include "core/peak/IntegrationRegion.h"

#include <Eigen/Core>

#include <QDialog>
#include <QDoubleSpinBox>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

class ColorButton;
class ColorMap;

namespace nsx {
class Peak3D;
}

struct PeakWindowParameters {
    double peak_end = 3.0;
    double bkg_begin = 3.0;
    double bkg_end = 6.0;
    QColor peak_color = Qt::yellow;
    QColor bkg_color = Qt::green;
    double alpha = 0.2;
    int max_intensity = 3000;
};

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class PeakWindow : public QDialog {

 public:
    PeakWindow(nsx::Peak3D* peak, QWidget* parent = nullptr);

    //! Refresh the whole dialog
    void refresh();

    QSize sizeHint() const;

 private:
    //! set up control widget
    void setControlWidgetUp();
    //! Plot the integration region image with overlay
    void drawFrame(QGraphicsView* view, std::size_t frame_index);
    //! Convert matrix to integration mask QImage
    QImage* getIntegrationMask(const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg);
    //! Set the peak description
    void setLabel();
    //! Remove this peak from the global vector
    void remove();
    //! Set up the initial display
    void initView();

    void grabParameters();
    void setParameters();

    void closeEvent(QCloseEvent* event) override;

    nsx::Peak3D* _peak;
    std::unique_ptr<nsx::IntegrationRegion> _integration_region;
    nsx::RegionData* _region_data;

    int _intensity;
    bool _logarithmic;
    std::vector<int> _index;

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

    static PeakWindowParameters _params;
};

#endif // NSX_GUI_SUBWINDOWS_PEAKWINDOW_H
