
//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/DirectBeamWidget.cpp
//! @brief     Implements class DirectBeamWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/DirectBeamWidget.h"

#include "gui/graphics/DetectorScene.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>

QVector<DirectBeamWidget*> DirectBeamWidget::_beam_widgets;

DirectBeamWidget::DirectBeamWidget()
    : QGridLayout()
    , _toggle_crosshair(new QCheckBox("Set initial direct beam position"))
    , _x_offset(new QDoubleSpinBox)
    , _y_offset(new QDoubleSpinBox)
    , _crosshair_size(new QSlider(Qt::Horizontal))
{
    _beam_widgets.push_back(this);

    _x_offset->setValue(0);
    _x_offset->setMinimum(-1000);
    _x_offset->setMaximum(1000);
    _x_offset->setDecimals(4);

    _y_offset->setValue(0);
    _y_offset->setMinimum(-1000);
    _y_offset->setMaximum(1000);
    _y_offset->setDecimals(4);

    _crosshair_size->setValue(20);
    _crosshair_size->setMaximum(100);
    _crosshair_size->setMinimum(5);

    int row = 0;
    addWidget(_toggle_crosshair, row++, 1, 1, 1);
    addLabel(row, "x offset", "Direct beam offset in x direction (pixels)");
    addWidget(_x_offset, row++, 1, 1, 1);
    addLabel(row, "y offset", "Direct beam offset in y direction (pixels)");
    addWidget(_y_offset, row++, 1, 1, 1);
    addLabel(row, "Crosshair size", "Radius of crosshair (pixels)");
    addWidget(_crosshair_size, row++, 1, 1, 1);

    toggleControlState();
}

DirectBeamWidget::DirectBeamWidget(DetectorScene* scene) : DirectBeamWidget()
{
    setDetectorScene(scene);
}

void DirectBeamWidget::setDetectorScene(DetectorScene* scene)
{
    _scene = scene;

    connect(
        _toggle_crosshair, &QCheckBox::stateChanged, this, &DirectBeamWidget::toggleControlState);
    connect(_toggle_crosshair, &QCheckBox::stateChanged, this, &DirectBeamWidget::toggleBeamSetter);
    connect(
        _x_offset, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &DirectBeamWidget::onSpinChanged);
    connect(
        _y_offset, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &DirectBeamWidget::onSpinChanged);

    connect(this, &DirectBeamWidget::beamPosChanged, scene, &DetectorScene::setBeamSetterPos);
    connect(scene, &DetectorScene::beamPosChanged, this, &DirectBeamWidget::onBeamPosChanged);
    connect(_crosshair_size, &QSlider::valueChanged, scene, &DetectorScene::onCrosshairResized);
}

QPointF DirectBeamWidget::pos() const
{
    return {_x_offset->value(), _y_offset->value()};
}

int DirectBeamWidget::crosshairSize() const
{
    return _crosshair_size->value();
}

void DirectBeamWidget::setSpinLimits(int ncols, int nrows)
{
    _x_offset->setMaximum(static_cast<double>(ncols) / 2.0);
    _x_offset->setMinimum(-static_cast<double>(ncols) / 2.0);
    _y_offset->setMaximum(static_cast<double>(nrows) / 2.0);
    _y_offset->setMinimum(-static_cast<double>(nrows) / 2.0);
}

void DirectBeamWidget::setEnabled(bool flag)
{
    _toggle_crosshair->setEnabled(flag);
    toggleControlState();
}

double DirectBeamWidget::xOffset() const
{
    return _x_offset->value();
}

double DirectBeamWidget::yOffset() const
{
    return _y_offset->value();
}

void DirectBeamWidget::onBeamPosChanged(QPointF pos)
{
    QSignalBlocker blocker1(_x_offset);
    QSignalBlocker blocker2(_y_offset);
    _x_offset->setValue(pos.x());
    _y_offset->setValue(pos.y());
    updateAllWidgets(pos);
}

void DirectBeamWidget::onSpinChanged()
{
    emit beamPosChanged({_x_offset->value(), _y_offset->value()});
}

void DirectBeamWidget::addLabel(int row, const QString& text, const QString& tooltip)
{
    auto* label = new QLabel(text);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setToolTip(tooltip);
    addWidget(label, row, 0, 1, 1);
}


void DirectBeamWidget::toggleBeamSetter()
{
    if (_toggle_crosshair->isChecked()) {
        QPointF offset(_x_offset->value(), _y_offset->value());
        _scene->addBeamSetter(offset, _crosshair_size->value());
    } else
        _scene->removeBeamSetter();
}

void DirectBeamWidget::toggleControlState()
{
    _x_offset->setEnabled(false);
    _y_offset->setEnabled(false);
    _crosshair_size->setEnabled(false);
    if (_toggle_crosshair->isChecked()) {
        _x_offset->setEnabled(true);
        _y_offset->setEnabled(true);
        _crosshair_size->setEnabled(true);
    }
}

void DirectBeamWidget::updateSpins(QPointF pos)
{
    QSignalBlocker blocker1(_x_offset);
    QSignalBlocker blocker2(_y_offset);
    _x_offset->setValue(pos.x());
    _y_offset->setValue(pos.y());
}

void DirectBeamWidget::updateAllWidgets(QPointF pos)
{
    for (auto* widget : _beam_widgets) {
        if (widget != this)
            widget->updateSpins(pos);
    }
}
