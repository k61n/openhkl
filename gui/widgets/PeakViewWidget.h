//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/PeakViewWidget.h
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

#include <QGridLayout>

class QCheckBox;
class QSpinBox;
class ColorButton;

class PeakViewWidget : public QGridLayout {
    Q_OBJECT

 public:
    PeakViewWidget(const QString& type1, const QString& type2);

 signals:
    void settingsChanged();

 private:
    void addHeadline(int row, const QString& type);
    void addLabel(int row, const QString& text);
    QCheckBox* addCheckBox(int row, int col, const QString& text, Qt::CheckState state);
    QSpinBox* addSpinBox(int row, int value);
    ColorButton* addColorButton(int row, int col, const QColor& color);

 public:
    QCheckBox* drawPeaks1;
    QCheckBox* drawBoxes1;
    QCheckBox* drawBkg1;
    QSpinBox* sizePeaks1;
    ColorButton* colorPeaks1;
    ColorButton* colorBoxes1;
    ColorButton* colorBkg1;

    QCheckBox* drawPeaks2;
    QCheckBox* drawBoxes2;
    QCheckBox* drawBkg2;
    QSpinBox* sizePeaks2;
    ColorButton* colorPeaks2;
    ColorButton* colorBoxes2;
    ColorButton* colorBkg2;
};

#endif // NSX_GUI_UTILITY_PEAKVIEWWIDGET_H
