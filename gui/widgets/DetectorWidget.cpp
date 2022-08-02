
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

#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

QList<DetectorWidget*> DetectorWidget::_detector_widgets = QList<DetectorWidget*>();

DetectorWidget::DetectorWidget(bool mode, bool cursor, bool slider, QWidget* parent)
    : QGridLayout(parent)
{
    QGridLayout* top_grid = new QGridLayout();
    QHBoxLayout* bottom_layout = new QHBoxLayout();

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

    _data_combo = new DataComboBox();
    _data_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _data_combo->setToolTip("Change the displayed data set");
    bottom_layout->addWidget(_data_combo);

    _scroll = new QScrollBar();
    _scroll->setOrientation(Qt::Horizontal);
    _scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _scroll->setToolTip("Scroll through the frames in this data set");
    bottom_layout->addWidget(_scroll);

    _spin = new QSpinBox();
    _spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _spin->setToolTip("Go to the numbered frame in this data set");
    bottom_layout->addWidget(_spin);

    addLayout(top_grid, 0, 0, 1, 1);
    addLayout(bottom_layout, 1, 0, 1, 1);

    connect(_scroll, &QScrollBar::valueChanged, scene(), &DetectorScene::slotChangeSelectedFrame);
    connect(_spin, QOverload<int>::of(&QSpinBox::valueChanged), _scroll, &QScrollBar::setValue);
    connect(_scroll, &QScrollBar::valueChanged, _spin, &QSpinBox::setValue);
    connect(
        _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWidget::refresh);
    connect(
        _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWidget::datasetChanged);
    connect(
        _spin, QOverload<int>::of(&QSpinBox::valueChanged), gGui->instrumentstate_window,
        &InstrumentStateWindow::onFrameChanged);

    setToolbarUp();
    bottom_layout->addWidget(_toolbar);

    if (mode) {
        _mode_combo = new QComboBox();
        _mode_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _mode_combo->setToolTip("Set the interaction mode for the detector image");
        bottom_layout->addWidget(_mode_combo);

        connect(
            _mode_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int i) { _detector_view->getScene()->changeInteractionMode(i); });
    }

    if (cursor) {
        _cursor_combo = new QComboBox();
        _cursor_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _cursor_combo->setToolTip("Set the cursor mode for the detector image");
        bottom_layout->addWidget(_cursor_combo);

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

void DetectorWidget::datasetChanged()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    auto data = _data_combo->currentData();
    scene()->removeBeamSetter(); // need to be sensitive of dataset change
    refresh();
}

void DetectorWidget::refresh()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    auto data = _data_combo->currentData();

    scene()->slotChangeSelectedData(data, _spin->value() - 1);
    scene()->clearPeakItems();
    scene()->drawPeakitems();
    scene()->update();
    _detector_view->fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    _scroll->setMinimum(1);
    _scroll->setMaximum(data->nFrames());

    _spin->setMinimum(1);
    _spin->setMaximum(data->nFrames());

    emit scene()->dataChanged();
}

void DetectorWidget::linkPeakModel(PeakCollectionModel* model1, PeakCollectionModel* model2)
{
    scene()->linkPeakModel1(model1);
    if (model2)
        scene()->linkPeakModel2(model2);
}

ohkl::sptrDataSet DetectorWidget::currentData()
{
    return _data_combo->currentData();
}

void DetectorWidget::changeView(int option)
{
    if (_detector_view == nullptr)
        return;
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

DetectorView* DetectorWidget::getDetectorView()
{
    return _detector_view;
}

void DetectorWidget::setToolbarUp()
{
    _toolbar = new QWidget;
    _toolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout* layout = new QHBoxLayout;

    _hide_masks = new QPushButton;
    _reset = new QPushButton;
    _copy_to_clipboard = new QPushButton;
    _save_to_file = new QPushButton;

    layout->addWidget(_hide_masks);
    layout->addWidget(_reset);
    layout->addWidget(_copy_to_clipboard);
    layout->addWidget(_save_to_file);

    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    _hide_masks->setMaximumWidth(_hide_masks->height());

    _hide_masks->setIcon(QIcon(path + "hide.svg"));
    _reset->setIcon(QIcon(path + "reset.svg"));
    _copy_to_clipboard->setIcon(QIcon(path + "copy.svg"));
    _save_to_file->setIcon(QIcon(path + "save.svg"));

    _hide_masks->setToolTip("Show/hide detector masks");
    _reset->setToolTip("Reset detector image");
    _copy_to_clipboard->setToolTip("Copy visible detector image to clipboard");
    _save_to_file->setToolTip("Save visible detector image to file");

    connect(
        _hide_masks, &QPushButton::clicked, _detector_view->getScene(), &DetectorScene::toggleMasks);

    connect(_reset, &QPushButton::clicked, _detector_view->getScene(), [=]() {
        _detector_view->getScene()->resetElements();
        _detector_view->getScene()->loadCurrentImage();
    });

    connect(_copy_to_clipboard, &QPushButton::clicked, this, [=]() {
        QPixmap pixMap = _detector_view->grab();
        QApplication::clipboard()->setImage(pixMap.toImage(), QClipboard::Clipboard);
    });

    connect(_save_to_file, &QPushButton::clicked, this, &DetectorWidget::saveScreenshot);

    _toolbar->setLayout(layout);
}

void DetectorWidget::saveScreenshot()
{
    QDateTime date = QDateTime::currentDateTime();
    QString fmt_date = date.toString("ddMMyy-hhmm");

    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();

    QString default_name =
        loadDirectory + "/" + QString::fromStdString(_data_combo->currentData()->name()) +
        "-" + fmt_date + ".png";

    QString file_path =
        QFileDialog::getSaveFileName(
            _detector_view, "Save image as", default_name, "Images (*.png *.jpg)");

    QFileInfo file_info(file_path);
    if (!file_info.absoluteFilePath().isNull()) {
        QPixmap pixMap = _detector_view->grab();
        pixMap.save(file_info.absoluteFilePath());
    }
}
