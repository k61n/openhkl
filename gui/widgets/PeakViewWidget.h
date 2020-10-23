//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utilities/PeakViewWidget.h
//! @brief     Defines class PeakViewWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_PEAKVIEWWIDGET_H
#define NSX_GUI_UTILITY_PEAKVIEWWIDGET_H

#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>

class ColorButton;

class PeakViewWidget : public QGridLayout {

 public:
    PeakViewWidget(std::string type1, std::string type2);

    QCheckBox* drawPeaks1();
    QCheckBox* drawBoxes1();
    QCheckBox* drawPeaks2();
    QCheckBox* drawBoxes2();
    ColorButton* peakColor1();
    ColorButton* boxColor1();
    ColorButton* peakColor2();
    ColorButton* boxColor2();
    QSpinBox* peakSize1();
    QSpinBox* boxSize1();
    QSpinBox* peakSize2();
    QSpinBox* boxSize2();

 private:
    //! Set up the GUI size policies
    void setSizePolicies();

    QSizePolicy* _size_policy_widgets;

    QCheckBox* _draw_peaks_1;
    QCheckBox* _draw_bbox_1;
    QSpinBox* _width_peaks_1;
    QSpinBox* _width_bbox_1;
    ColorButton* _color_peaks_1;
    ColorButton* _color_bbox_1;

    QCheckBox* _draw_peaks_2;
    QCheckBox* _draw_bbox_2;
    QSpinBox* _width_peaks_2;
    QSpinBox* _width_bbox_2;
    ColorButton* _color_peaks_2;
    ColorButton* _color_bbox_2;
};

#endif // NSX_GUI_UTILITY_PEAKVIEWWIDGET_H
