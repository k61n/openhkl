
//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/DetectorWidget.cpp
//! @brief     Implements class DetectorWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/DetectorWidget.h"

#include "core/data/DataSet.h"
#include "gui/MainWin.h" // gGui
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h" // gSession
#include "gui/subwindows/InstrumentStateWindow.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/LinkedComboBox.h"

#include <QComboBox>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

QList<DetectorWidget*> DetectorWidget::_detector_widgets = QList<DetectorWidget*>();

DetectorWidget::DetectorWidget(bool mode, bool cursor, bool slider, QWidget* parent)
    : QGridLayout(parent)
{
    QGridLayout* top_grid = new QGridLayout();
    QGridLayout* bottom_grid = new QGridLayout();

    _detector_view = new DetectorView();
    _detector_view->scale(1, -1);
    top_grid->addWidget(_detector_view, 0, 0, 1, 1);

    if (slider) {
        _has_slider = true;
        _intensity_slider = new QSlider(Qt::Vertical);
        _intensity_slider->setMouseTracking(true);
        _intensity_slider->setMinimum(1);
        _intensity_slider->setMaximum(10000);
        _intensity_slider->setValue(scene()->intensity());
        _intensity_slider->setSingleStep(1);
        _intensity_slider->setOrientation(Qt::Vertical);
        _intensity_slider->setTickPosition(QSlider::TicksRight);
        _intensity_slider->setToolTip("Adjust the image intensity scale");
        top_grid->addWidget(_intensity_slider, 0, 1, 1, 1);
    }

    int col = 0;

    _data_combo = new DataComboBox();
    _data_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _data_combo->setToolTip("Change the displayed data set");
    bottom_grid->addWidget(_data_combo, 0, ++col, 1, 1);

    _scroll = new QScrollBar();
    _scroll->setOrientation(Qt::Horizontal);
    _scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _scroll->setToolTip("Scroll through the frames in this data set");
    bottom_grid->addWidget(_scroll, 0, ++col, 1, 1);

    _spin = new QSpinBox();
    _spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _spin->setToolTip("Go to the numbered frame in this data set");
    bottom_grid->addWidget(_spin, 0, ++col, 1, 1);

    addLayout(top_grid, 0, 0, 1, 1);
    addLayout(bottom_grid, 1, 0, 1, 1);

    connect(_scroll, &QScrollBar::valueChanged, scene(), &DetectorScene::slotChangeSelectedFrame);
    connect(_spin, QOverload<int>::of(&QSpinBox::valueChanged), _scroll, &QScrollBar::setValue);
    connect(_scroll, &QScrollBar::valueChanged, _spin, &QSpinBox::setValue);
    connect(
        _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWidget::refresh);
    connect(
        _spin, QOverload<int>::of(&QSpinBox::valueChanged), gGui->instrumentstate_window,
        &InstrumentStateWindow::onFrameChanged);


    if (mode) {
        _mode_combo = new QComboBox();
        _mode_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _mode_combo->setToolTip("Set the interaction mode for the detector image");
        bottom_grid->addWidget(_mode_combo, 0, ++col, 1, 1);

        connect(
            _mode_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int i) { _detector_view->getScene()->changeInteractionMode(i); });
    }

    if (cursor) {
        _cursor_combo = new QComboBox();
        _cursor_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _cursor_combo->setToolTip("Set the cursor mode for the detector image");
        bottom_grid->addWidget(_cursor_combo, 0, ++col, 1, 1);

        connect(
            _cursor_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int i) { _detector_view->getScene()->changeCursorMode(i); });
    }

    syncIntensitySliders();
    _detector_widgets.push_back(this);
}

void DetectorWidget::syncIntensitySliders()
{
    if (_has_slider) {
        for (auto* widget : _detector_widgets) {
            connect(
                _intensity_slider, &QSlider::valueChanged, widget->scene(),
                &DetectorScene::setMaxIntensity);
            if (widget->hasSlider()) {
                connect(
                    widget->slider(), &QSlider::valueChanged, scene(),
                    &DetectorScene::setMaxIntensity);
                connect(
                    _intensity_slider, &QSlider::valueChanged, widget->slider(),
                    &QSlider::setValue);
                connect(
                    widget->slider(), &QSlider::valueChanged, _intensity_slider,
                    &QSlider::setValue);
            }
        }
    }
}

void DetectorWidget::refresh()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    auto data = _data_combo->currentData();

    scene()->slotChangeSelectedData(data, _spin->value());
    scene()->clearPeakItems();
    scene()->drawPeakitems();
    scene()->update();
    _detector_view->fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    _scroll->setMinimum(0);
    _scroll->setMaximum(data->nFrames());

    _spin->setMinimum(0);
    _spin->setMaximum(data->nFrames());

    emit scene()->dataChanged();
}

void DetectorWidget::linkPeakModel(PeakCollectionModel* model1, PeakCollectionModel* model2)
{
    scene()->linkPeakModel1(model1);
    if (model2)
        scene()->linkPeakModel2(model2);
}

nsx::sptrDataSet DetectorWidget::currentData()
{
    return _data_combo->currentData();
}

void DetectorWidget::changeView(int option)
{
    if (_detector_view==nullptr) return;
    QTransform trans;
    trans.scale(-1, -1); // fromDetector (default; 0)
    if (option == 1) // fromSample
        trans.scale(-1, 1);
    _detector_view->setTransform(trans);
    _detector_view->fitScene();
}

DetectorScene* DetectorWidget::scene()
{
    return _detector_view->getScene();
}

QSpinBox* DetectorWidget::spin()
{
    return _spin;
}

QScrollBar* DetectorWidget::scroll()
{
    return _scroll;
}

DataComboBox* DetectorWidget::dataCombo()
{
    return _data_combo;
}

QComboBox* DetectorWidget::modeCombo()
{
    return _mode_combo;
}

QComboBox* DetectorWidget::cursorCombo()
{
    return _cursor_combo;
}

QSlider* DetectorWidget::slider()
{
    return _intensity_slider;
}

bool DetectorWidget::hasSlider()
{
    return _has_slider;
}
