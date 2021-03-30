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

#include "gui/utility/ColorButton.h"

class ColorButton;

class PeakViewWidget : public QGridLayout {

 public:
    PeakViewWidget(const QString& type1, const QString& type2);

    QCheckBox* drawPeaks1() { return _draw_peaks_1; };
    QCheckBox* drawBoxes1() { return _draw_bbox_1; };
    QCheckBox* drawBkg1() { return _draw_bkg_1; };
    QCheckBox* drawPeaks2() { return _draw_peaks_2; };
    QCheckBox* drawBoxes2() { return _draw_bbox_2; };
    QCheckBox* drawBkg2() { return _draw_bkg_2; };
    ColorButton* peakColor1() { return _color_peaks_1; };
    ColorButton* boxColor1() { return _color_bbox_1; };
    ColorButton* bkgColor1() { return _color_bkg_1; };
    ColorButton* peakColor2() { return _color_peaks_2; };
    ColorButton* boxColor2() { return _color_bbox_2; };
    ColorButton* bkgColor2() { return _color_bkg_2; };
    QSpinBox* peakSize1() { return _width_peaks_1; };
    QSpinBox* peakSize2() { return _width_peaks_1; };
    void setPeakColor1(QColor color) { _color_peaks_1->changeColor(color); };
    void setPeakColor2(QColor color) { _color_peaks_2->changeColor(color); };
    void setBoxColor1(QColor color) { _color_bbox_1->changeColor(color); };
    void setBoxColor2(QColor color) { _color_bbox_2->changeColor(color); };
    void setBkgColor1(QColor color) { _color_bkg_1->changeColor(color); };
    void setBkgColor2(QColor color) { _color_bkg_2->changeColor(color); };

 private:
    QCheckBox* _draw_peaks_1;
    QCheckBox* _draw_bbox_1;
    QCheckBox* _draw_bkg_1;
    QSpinBox* _width_peaks_1;
    ColorButton* _color_peaks_1;
    ColorButton* _color_bbox_1;
    ColorButton* _color_bkg_1;

    QCheckBox* _draw_peaks_2;
    QCheckBox* _draw_bbox_2;
    QCheckBox* _draw_bkg_2;
    QSpinBox* _width_peaks_2;
    ColorButton* _color_peaks_2;
    ColorButton* _color_bbox_2;
    ColorButton* _color_bkg_2;
};

#endif // NSX_GUI_UTILITY_PEAKVIEWWIDGET_H
