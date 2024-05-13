//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/PeakWindow.cpp
//! @brief     Implements class PeakWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_PEAKWINDOW_H
#define OHKL_GUI_SUBWINDOWS_PEAKWINDOW_H

#include "core/peak/IntegrationRegion.h"

#include <QDialog>

class ColorButton;
class ColorMap;
class QComboBox;
class QDoubleSpinBox;
class QGraphicsView;
class QGridLayout;
class QSlider;

namespace ohkl {
class Peak3D;
class RegionData;
}

struct PeakWindowParameters {
    double peak_end = 3.0;
    double bkg_begin = 3.0;
    double bkg_end = 6.0;
    double fixed_peak_end = 8.0;
    double fixed_bkg_begin = 1.0;
    double fixed_bkg_end = 2.0;
    QColor peak_color = Qt::yellow;
    QColor bkg_color = Qt::green;
    double alpha = 0.2;
    int max_intensity = 3000;
    ohkl::RegionType region_type = ohkl::RegionType::VariableEllipsoid;
};

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class PeakWindow : public QDialog {

 public:
    PeakWindow(ohkl::Peak3D* peak, QWidget* parent = nullptr);

    //! Refresh the whole dialog
    void refresh();

    QSize sizeHint() const override;

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

    //! Set widgets from parameters structure
    void grabParameters();
    //! Set parameters structure from widgets
    void setParameters();

    void closeEvent(QCloseEvent* event) override;

    ohkl::Peak3D* _peak;
    std::unique_ptr<ohkl::IntegrationRegion> _integration_region;
    ohkl::RegionData* _region_data;

    bool _logarithmic;

    ColorMap* _colormap;
    QVector<QGraphicsView*> _views;
    QGridLayout* _grid_layout;
    QGridLayout* _control_layout;

    QDoubleSpinBox* _peak_end;
    QDoubleSpinBox* _bkg_begin;
    QDoubleSpinBox* _bkg_end;
    QComboBox* _region_type;
    QDoubleSpinBox* _alpha;
    ColorButton* _peak_color_button;
    ColorButton* _bkg_color_button;
    QSlider* _intensity_slider;

    static PeakWindowParameters _params;
};

#endif // OHKL_GUI_SUBWINDOWS_PEAKWINDOW_H
