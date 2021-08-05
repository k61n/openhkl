
//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include "gui/MainWin.h" // gGui
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/utility/LinkedComboBox.h"

#include <QComboBox>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

DetectorWidget::DetectorWidget(bool data, bool mode, bool slider, QWidget* parent)
    : QGridLayout(parent)
{
    QGridLayout* top_grid = new QGridLayout();
    QGridLayout* bottom_grid = new QGridLayout();

    _detector_view = new DetectorView();
    _detector_view->scale(1, -1);
    top_grid->addWidget(_detector_view, 0, 0, 1, 1);

    if (slider) {
        _intensity_slider = new QSlider(Qt::Vertical);
        _intensity_slider->setMouseTracking(true);
        _intensity_slider->setMinimum(1);
        _intensity_slider->setMaximum(10000);
        _intensity_slider->setValue(scene()->intensity();
        _intensity_slider->setSingleStep(1);
        _intensity_slider->setOrientation(Qt::Vertical);
        _intensity_slider->setTickPosition(QSlider::TicksRight);
        _intensity_slider->setToolTip("Adjust the image intensity scale");
        top_grid->addWidget(_intensity_slider, 0, 0, 1, 1);
    }

    int col = 0;

    if (data) {
        _data_combo = new LinkedComboBox(ComboType::DataSet, gGui->sentinel);
        _data_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        _data_combo->setToolTip("Change the displayed data set");
        bottom_grid->addWidget(_data_combo, 0, ++col, 1, 1);
    }

    _scroll = new QScrollBar();
    _scroll->setOrientation(Qt::Horizontal);
    _scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _scroll->setToolTip("Scroll through the frames in this data set");
    bottom_grid->addWidget(_scroll, 0, ++col, 1, 1);

    _spin = new QSpinBox();
    _spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _spin->setToolTip("Go to the numbered frame in this data set");
    bottom_grid->addWidget(_spin, 0, ++col, 1, 1);

    if (mode) {
        _mode_combo = new QComboBox();
        _mode_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _mode_combo->setToolTip("Set the cursor mode for the detector image");
        bottom_grid->addWidget(_mode_combo, 0, ++col, 1, 1);
    }

    addLayout(top_grid, 0, 0, 1, 1);
    addLayout(bottom_grid, 1, 0, 1, 1);

    connect(_scroll, &QScrollBar::valueChanged, scene(), &DetectorScene::slotChangeSelectedFrame);
    connect(_spin, QOverload<int>::of(&QSpinBox::valueChanged), _scroll, &QScrollBar::setValue);
    connect(_scroll, &QScrollBar::valueChanged, _spin, &QSpinBox::setValue);

    if (data)
        connect(
            _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &DetectorWidget::refresh);

    if (mode)
        connect(
            _mode_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int i) { _detector_view->getScene()->changeCursorMode(i); });
}

void DetectorWidget::updateDatasetList(const std::vector<nsx::sptrDataSet>& data_list)
{
    _data_list = data_list;

    _data_combo->blockSignals(true);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();

    if (!data_list.empty()) {
        for (auto item : _data_list)
            _data_combo->addItem(QString::fromStdString(item->name()));
        _data_combo->setCurrentText(current_data);
    }
    _data_combo->blockSignals(false);
}

void DetectorWidget::refresh()
{
    if (_data_combo->count() == 0)
        return;

    auto data = _data_list.at(_data_combo->currentIndex());
    scene()->slotChangeSelectedData(data, _spin->value());
    scene()->update();

    _scroll->setMinimum(0);
    _scroll->setMaximum(data->nFrames());

    _spin->setMinimum(0);
    _spin->setMaximum(data->nFrames());
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

LinkedComboBox* DetectorWidget::dataCombo()
{
    return _data_combo;
}

QComboBox* DetectorWidget::modeCombo()
{
    return _mode_combo;
}

QSlider* DetectorWidget::slider()
{
    return _intensity_slider;
}
