//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/ImagePanel.cpp
//! @brief     Implements classes ImagePanel, SubframeImage
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/ImagePanel.h"

#include "core/data/DataSet.h"
#include "gui/MainWin.h"
#include "gui/actions/Actions.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"

#include <QGraphicsView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSplitter>
#include <QTreeView>

ImagePanel::ImagePanel()
{
    QGridLayout* main_layout = new QGridLayout(this);
    QGridLayout* top_layout = new QGridLayout;

    _image_view = new DetectorView();
    _scrollbar = new QScrollBar();
    _slider = new QSlider(Qt::Vertical);
    _frame = new QSpinBox();
    _mode = new QComboBox();

    _image_view->getScene()->setMaxIntensity(5000);
    _image_view->scale(1, -1);

    _scrollbar->setMouseTracking(true);
    _scrollbar->setFocusPolicy(Qt::WheelFocus);
    _scrollbar->setOrientation(Qt::Horizontal);
    _scrollbar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    _slider->setMouseTracking(true);
    _slider->setMinimum(1);
    _slider->setMaximum(10000);
    _slider->setValue(5000);
    _slider->setSingleStep(1);
    _slider->setOrientation(Qt::Vertical);
    _slider->setTickPosition(QSlider::TicksRight);
    _slider->setToolTip("Adjust the image intensity scale");

    _mode->addItems(
        QStringList{"Zoom", "Selection", "Rectangular mask", "Elliptical mask", "Line plot",
            "Horizontal slice", "Vertical slice"});

    _frame->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    top_layout->addWidget(_image_view, 0, 0, 1, 1);
    top_layout->addWidget(_slider, 0, 1, 1, 1);

    main_layout->addLayout(top_layout, 0, 0, 1, 3);
    main_layout->addWidget(_scrollbar, 1, 0, 1, 1);
    main_layout->addWidget(_frame, 1, 1, 1, 1);
    main_layout->addWidget(_mode, 1, 2, 1, 1);

    connect(
        _scrollbar, &QScrollBar::valueChanged, _image_view->getScene(),
        &DetectorScene::slotChangeSelectedFrame);

    connect(_scrollbar, &QScrollBar::valueChanged, [=](int i) {
        _frame->blockSignals(true);
        _frame->setValue(i);
        _frame->blockSignals(false);
        _image_view->getScene()->slotChangeSelectedFrame(i);
    });

    connect(_frame, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int i) {
        _scrollbar->blockSignals(true);
        _scrollbar->setValue(i);
        _scrollbar->blockSignals(false);
        _image_view->getScene()->slotChangeSelectedFrame(i);
    });

    connect(
        _mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int i) { _image_view->getScene()->changeInteractionMode(i); });
}

void ImagePanel::dataChanged(int numor)
{
    _mode->setEnabled(false);
    _slider->setEnabled(false);
    _scrollbar->setEnabled(false);
    _frame->setEnabled(false);

    _image_view->getScene()->resetScene();
    _image_view->fitScene();

    if (gSession->currentProjectNum() >= 0) {
        nsx::sptrDataSet dataset = gSession->currentProject()->getData(numor);
        if (dataset) {
            _mode->setEnabled(true);
            _slider->setEnabled(true);
            _scrollbar->setEnabled(true);
            _frame->setEnabled(true);

            _scrollbar->setMaximum(dataset->nFrames());
            _scrollbar->setMinimum(0);
            _scrollbar->setSingleStep(1);

            _frame->setMaximum(dataset->nFrames());
            _frame->setMinimum(0);
            _frame->setSingleStep(1);

            _image_view->getScene()->slotChangeSelectedData(dataset, 0);
        }
    }
}

void ImagePanel::changeView(int option)
{
    QTransform trans;
    trans.scale(-1, -1); // fromDetector (default; 0)
    if (option == 1) // fromSample
        trans.scale(-1, 1);
    _image_view->setTransform(trans);
    _image_view->fitScene();
}
