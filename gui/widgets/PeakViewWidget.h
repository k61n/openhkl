//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PeakViewWidget.h
//! @brief     Defines class PeakViewWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_PEAKVIEWWIDGET_H
#define OHKL_GUI_UTILITY_PEAKVIEWWIDGET_H

#include "core/integration/IIntegrator.h"

#include <QGridLayout>

class QCheckBox;
class QComboBox;
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
        QComboBox* regionType;
        QSpinBox* sizePeaks;
        QDoubleSpinBox* alphaIntegrationRegion;
        ColorButton* colorPeaks;
        ColorButton* colorIntPeak;
        ColorButton* colorIntBkg;
        QCheckBox* previewIntRegion;
        QDoubleSpinBox* peakEnd;
        QDoubleSpinBox* bkgBegin;
        QDoubleSpinBox* bkgEnd;

        ohkl::IntegrationParameters params;

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
    QComboBox* addCombo(int row);
    ColorButton* addColorButton(int row, int col, const QColor& color);

    void switchIntRegionType();
};

#endif // OHKL_GUI_UTILITY_PEAKVIEWWIDGET_H
