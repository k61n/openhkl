//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/DirectBeamWidget.h
//! @brief     Defines class DirectBeamWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_DIRECTBEAMWIDGET_H
#define OHKL_GUI_UTILITY_DIRECTBEAMWIDGET_H

#include <QGridLayout>

class DetectorScene;
class QCheckBox;
class QDoubleSpinBox;
class QSlider;

class DirectBeamWidget : public QGridLayout {
    Q_OBJECT

 public:
    DirectBeamWidget();
    DirectBeamWidget(DetectorScene* scene);

    //! Set the detector scene
    void setDetectorScene(DetectorScene* scene);
    //! Return the position of the beam setter
    QPointF pos() const;
    //! Return the crosshair size
    int crosshairSize() const;
    //! Set the minima and maxima of the spin boxes
    void setSpinLimits(int ncols, int nrows);
    //! Enable/disable this widget
    void setEnabled(bool flag);
    //! Return the checkbox widget
    QCheckBox* crosshairOn() const { return _toggle_crosshair; };
    //! Return the x offset
    double xOffset() const;
    //! Return the y offset
    double yOffset() const;

 public slots:
    //! Modify offset spin boxes when DetectorScene beam position changes
    void onBeamPosChanged(QPointF pos);
    //! Modify DetectorScene when offset spin boxes are changed
    void onSpinChanged();
    //! Toggle the beam setter crosshair
    void toggleBeamSetter();
    //! Enable/disable the controls
    void toggleControlState();

 signals:
    //! Emitted when the beam position changes
    void beamPosChanged(QPointF pos);

 private:
    void addLabel(int row, const QString& text, const QString& tooltip);
    void updateSpins(QPointF pos);
    void updateAllWidgets(QPointF pos);

    QCheckBox* _toggle_crosshair;
    QDoubleSpinBox* _x_offset;
    QDoubleSpinBox* _y_offset;
    QSlider* _crosshair_size;

    DetectorScene* _scene;

    static QVector<DirectBeamWidget*> _beam_widgets;
};

#endif // OHKL_GUI_UTILITY_DIRECTBEAMWIDGET_H
