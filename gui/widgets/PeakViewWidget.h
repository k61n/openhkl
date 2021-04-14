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
class QDoubleSpinBox;
class QSpinBox;
class ColorButton;

class PeakViewWidget : public QGridLayout {
    Q_OBJECT

 public:
    PeakViewWidget(const QString& titleSet1, const QString& titleSet2);

    struct Set {
        QCheckBox* drawPeaks;
        QCheckBox* drawBoxes;
        QCheckBox* drawBkg;
        QCheckBox* drawIntegrationRegion;
        QSpinBox* sizePeaks;
        QDoubleSpinBox* alphaIntegrationRegion;
        ColorButton* colorPeaks;
        ColorButton* colorBoxes;
        ColorButton* colorBkg;
        ColorButton* colorIntPeak;
        ColorButton* colorIntBkg;

        //! Set the color of all three color buttons at once
        void setColor(const QColor& color);
        void setIntegrationRegionColors(const QColor& peak, const QColor& bkg, double alpha);

    } set1, set2;

 signals:
    //! Emitted whenever any of the settings changed
    void settingsChanged();

 private:
    void createSet(Set& set, const QString& title, const QColor& btnColor);
    void addIntegrationRegion(Set& set, const QColor& peak, const QColor& bkg);
    void addHeadline(int row, const QString& type);
    void addLabel(int row, const QString& text);
    QCheckBox* addCheckBox(int row, int col, const QString& text, Qt::CheckState state);
    QSpinBox* addSpinBox(int row, int value);
    QDoubleSpinBox* addDoubleSpinBox(int row, double value);
    ColorButton* addColorButton(int row, int col, const QColor& color);
};

#endif // NSX_GUI_UTILITY_PEAKVIEWWIDGET_H
