
//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/DetectorWidget.cpp
//! @brief     Implements class DetectorWidget
//!
//! @homepage  https://openhkl.org
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
#include "gui/widgets/PeakViewWidget.h"

#include <QButtonGroup>
#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <qpushbutton.h>

QList<DetectorWidget*> DetectorWidget::_detector_widgets = QList<DetectorWidget*>();

DetectorWidget::DetectorWidget(
    std::size_t max_collections, bool cursor, bool slider, QWidget* parent)
    : QGridLayout(parent)
{
    QGridLayout* top_grid = new QGridLayout();
    QHBoxLayout* bottom_layout = new QHBoxLayout();

    _detector_view = new DetectorView(max_collections);
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

    if (cursor) {
        _cursor_combo = new QComboBox();
        _cursor_combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _cursor_combo->setToolTip("Set the cursor mode for the detector image");
        _cursor_combo->addItems(
            QStringList{"Cursor mode", "Pixel", "\u03B8", "\u03B3/\u03BD", "d", "Miller Indices"});
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

    ohkl::sptrDataSet new_data = _data_combo->currentData();
    scene()->slotChangeSelectedData(new_data, _spin->value());
    scene()->removeBeamSetter(); // need to be sensitive of dataset change
    refresh();
}

void DetectorWidget::refresh()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    ohkl::sptrDataSet data = _data_combo->currentData();

    _hide_masks->setChecked(!scene()->params()->masks);

    scene()->slotChangeSelectedData(data, _spin->value());
    scene()->clearPeakItems();
    scene()->drawPeakItems();
    scene()->update();
    _detector_view->fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    _scroll->setMinimum(1);
    _scroll->setMaximum(data->nFrames());

    _spin->setMinimum(1);
    _spin->setMaximum(data->nFrames());

    emit scene()->dataChanged();
}

void DetectorWidget::linkPeakModel(
    PeakCollectionModel* model, PeakViewWidget* widget, std::size_t idx)
{
    scene()->linkPeakModel(model, widget, idx);
}

void DetectorWidget::setVisualisationMode(VisualisationType vtype)
{
    scene()->setVisualisationMode(vtype);
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

    _gradient = new QPushButton;
    _hide_masks = new QPushButton;
    _peak_labels = new QPushButton;
    _copy_to_clipboard = new QPushButton;
    _save_to_file = new QPushButton;
    _zoom = new QPushButton;
    _select = new QPushButton;

    // only allow one cursor mode with button group
    _cursor_mode_buttons = new QButtonGroup;
    _cursor_mode_buttons->addButton(_zoom);
    _cursor_mode_buttons->addButton(_select);
    _cursor_mode_buttons->setExclusive(true);

    _gradient->setMaximumWidth(_gradient->height());
    _gradient->setCheckable(true);
    _gradient->setChecked(false);

    _hide_masks->setCheckable(true);
    _hide_masks->setChecked(true);

    _peak_labels->setCheckable(true);
    _peak_labels->setChecked(false);

    _zoom->setCheckable(true);
    _zoom->setChecked(true);

    _select->setCheckable(true);
    _select->setChecked(false);

    layout->addWidget(_gradient);
    layout->addWidget(_hide_masks);
    layout->addWidget(_peak_labels);
    layout->addSpacing(30);
    layout->addWidget(_copy_to_clipboard);
    layout->addWidget(_save_to_file);
    layout->addWidget(_zoom);
    layout->addWidget(_select);

    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    _gradient->setIcon(QIcon(path + "gradient.svg"));
    _hide_masks->setIcon(QIcon(path + "hide.svg"));
    _peak_labels->setIcon(QIcon(path + "layers.svg"));
    _copy_to_clipboard->setIcon(QIcon(path + "copy.svg"));
    _save_to_file->setIcon(QIcon(path + "save.svg"));
    _zoom->setIcon(QIcon(path + "zoom.svg"));
    _select->setIcon(QIcon(path + "select.svg"));

    _gradient->setToolTip("Toggle magnitude of gradient of image");
    _hide_masks->setToolTip("Show/hide detector masks");
    _peak_labels->setToolTip("Show/hide Miller index labels");
        _copy_to_clipboard->setToolTip("Copy visible detector image to clipboard");
    _save_to_file->setToolTip("Save visible detector image to file");
    _zoom->setToolTip("Enable zoom cursor on detector image");
    _select->setToolTip("Enable rectangle select cursor on detector image");

    connect(_gradient, &QPushButton::clicked, this, &DetectorWidget::toggleGradient);
    connect(_hide_masks, &QPushButton::clicked, this, &DetectorWidget::toggleMasks);
    connect(_peak_labels, &QPushButton::clicked, this, &DetectorWidget::toggleLabels);
    connect(_copy_to_clipboard, &QPushButton::clicked, this, [=]() {
        QPixmap pixMap = _detector_view->grab();
        QApplication::clipboard()->setImage(pixMap.toImage(), QClipboard::Clipboard);
    });
    connect(_save_to_file, &QPushButton::clicked, this, &DetectorWidget::saveScreenshot);
    connect(_zoom, &QPushButton::clicked, this, &DetectorWidget::toggleCursorMode);
    connect(_select, &QPushButton::clicked, this, &DetectorWidget::toggleCursorMode);

    _toolbar->setLayout(layout);
}

void DetectorWidget::saveScreenshot()
{
    QDateTime date = QDateTime::currentDateTime();
    QString fmt_date = date.toString("ddMMyy-hhmm");

    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();

    QString default_name = loadDirectory + "/"
        + QString::fromStdString(_data_combo->currentData()->name()) + "-" + fmt_date + ".png";

    QString file_path = QFileDialog::getSaveFileName(
        _detector_view, "Save image as", default_name, "Images (*.png *.jpg)");

    QFileInfo file_info(file_path);
    if (!file_info.absoluteFilePath().isNull()) {
        QPixmap pixMap = _detector_view->grab();
        pixMap.save(file_info.absoluteFilePath());
    }
}

void DetectorWidget::toggleCursorMode()
{
    if (_zoom->isChecked())
        scene()->changeInteractionMode(0);
    else if (_select->isChecked())
        scene()->changeInteractionMode(1);
}

void DetectorWidget::enableCursorMode(bool enable)
{
    _zoom->setEnabled(enable);
    _select->setEnabled(enable);
}

void DetectorWidget::toggleGradient()
{
    scene()->params()->gradient = _gradient->isChecked();
    scene()->loadCurrentImage();
}

void DetectorWidget::toggleMasks()
{
    scene()->params()->masks = !_hide_masks->isChecked();
    scene()->loadCurrentImage();
}

void DetectorWidget::toggleLabels()
{
    scene()->params()->labels = _peak_labels->isChecked();
    scene()->drawPeakItems();
}
