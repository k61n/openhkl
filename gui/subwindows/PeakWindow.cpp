//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/PeakWindow.h
//! @brief     Defines class PeakWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/PeakWindow.h"

#include "core/peak/Peak3D.h"
#include "gui/MainWin.h" // gGui
#include "gui/models/ColorMap.h"
#include "gui/utility/ColorButton.h"

#include <QLineEdit>
#include <QPixmap>
#include <QTextStream>
#include <QVBoxLayout>

PeakWindowParameters PeakWindow::_params = {};

PeakWindow::PeakWindow(nsx::Peak3D* peak, QWidget* parent /* = nullptr */)
    : QDialog(parent)
    , _peak(peak)
    , _integration_region(nullptr)
    , _intensity(3000)
    , _logarithmic(false)
    , _colormap(new ColorMap())
{
    setModal(false);
    setControlWidgetUp();

    QWidget* view_widget = new QWidget;
    QWidget* control_widget = new QWidget;
    QScrollArea* scroll_area = new QScrollArea;
    QVBoxLayout* main_layout = new QVBoxLayout;

    setLayout(main_layout);
    main_layout->addWidget(scroll_area);

    control_widget->setLayout(_control_layout);

    main_layout->addWidget(view_widget);
    main_layout->addWidget(control_widget);

    _grid_layout = new QGridLayout;

    scroll_area->setWidget(view_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view_widget->setLayout(_grid_layout);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    view_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    gGui->peak_windows.push_back(this);

    initView();
}

void PeakWindow::setControlWidgetUp()
{
    _control_layout = new QGridLayout;

    _peak_end = new QDoubleSpinBox;
    _bkg_begin = new QDoubleSpinBox;
    _bkg_end = new QDoubleSpinBox;
    _peak_color_button = new ColorButton;
    _bkg_color_button = new ColorButton;
    _alpha = new QDoubleSpinBox;
    _intensity_slider = new QSlider;

    _intensity_slider->setOrientation(Qt::Horizontal);
    _intensity_slider->setMouseTracking(true);
    _intensity_slider->setMinimum(1);
    _intensity_slider->setMaximum(10000);
    _intensity_slider->setSingleStep(1);

    grabParameters();

    _peak_end->setSingleStep(0.1);
    _bkg_begin->setSingleStep(0.1);
    _bkg_end->setSingleStep(0.1);
    _alpha->setSingleStep(0.1);

    QLabel* label = new QLabel;
    label->setText("Peak end");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 1, 1, 1);
    _control_layout->addWidget(_peak_end, 0, 2, 1, 1);

    label = new QLabel;
    label->setText("Bkg begin");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 3, 1, 1);
    _control_layout->addWidget(_bkg_begin, 0, 4, 1, 1);

    label = new QLabel;
    label->setText("Bkg end");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 5, 1, 1);
    _control_layout->addWidget(_bkg_end, 0, 6, 1, 1);

    label = new QLabel;
    label->setText("Peak colour");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 7, 1, 1);
    _control_layout->addWidget(_peak_color_button, 0, 8, 1, 1);

    label = new QLabel;
    label->setText("Bkg colour");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 9, 1, 1);
    _control_layout->addWidget(_bkg_color_button, 0, 10, 1, 1);

    label = new QLabel;
    label->setText("Alpha");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 11, 1, 1);
    _control_layout->addWidget(_alpha, 0, 12, 1, 1);

    label = new QLabel;
    label->setText("Intensity max");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _control_layout->addWidget(label, 0, 13, 1, 1);
    _control_layout->addWidget(_intensity_slider, 0, 14, 1, 3);

    connect(
        _peak_end, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &PeakWindow::refresh);
    connect(
        _bkg_begin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &PeakWindow::refresh);
    connect(
        _bkg_end, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &PeakWindow::refresh);
    connect(
        _alpha, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &PeakWindow::refresh);
    connect(_intensity_slider, &QSlider::valueChanged, this, &PeakWindow::refresh);
    connect(_peak_color_button, &ColorButton::colorChanged, this, &PeakWindow::refresh);
    connect(_bkg_color_button, &ColorButton::colorChanged, this, &PeakWindow::refresh);
}

void PeakWindow::initView()
{
    _integration_region = std::make_unique<nsx::IntegrationRegion>(
        _peak, _params.peak_end, _params.bkg_begin, _params.bkg_end);
    _region_data = _integration_region->getRegion();
    for (std::size_t i = 0; i < _region_data->nFrames(); ++i) {
        _index.push_back(_region_data->index(i));
        QGraphicsView* view = new QGraphicsView;
        drawFrame(view, i);
        if (view) {
            view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
            view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->scale(0.3, 0.3);
            _views.push_back(view);
            _grid_layout->addWidget(view, 0, i, 1, 1);
        }
    }
    if (_views.size() == 0) {
        remove();
        throw std::runtime_error("Invalid integration region");
    }
    setLabel();
}

void PeakWindow::refresh()
{
    setParameters();
    _integration_region.reset();
    _integration_region = std::make_unique<nsx::IntegrationRegion>(
        _peak, _params.peak_end, _params.bkg_begin, _params.bkg_end);
    _region_data = _integration_region->getRegion();
    for (int i = 0; i < _index.size(); ++i) {
        try {
            int j = _region_data->getRegionDataIndex(_index[i]);
            QGraphicsView* view = _views[i];
            drawFrame(view, j);
        } catch (std::range_error& e) {
            continue;
        }
    }
}

void PeakWindow::drawFrame(QGraphicsView* view, std::size_t frame_index)
{
    QRect rect(0, 0, _region_data->cols() + 1, _region_data->rows() + 1);
    if (!view->scene())
        view->setScene(new QGraphicsScene());
    view->scene()->clear();
    view->scene()->setSceneRect(rect);

    // add the image data
    try {
        QGraphicsPixmapItem* image =
            view->scene()->addPixmap(QPixmap::fromImage(_colormap->matToImage(
                _region_data->frame(frame_index).cast<double>(), rect, _params.max_intensity,
                _logarithmic)));
        image->setZValue(-2);
    } catch (std::range_error& e) {
        return;
    }

    // add the integration overlay
    QColor peak_color = _params.peak_color;
    QColor bkg_color = _params.bkg_color;
    peak_color.setAlphaF(_params.alpha);
    bkg_color.setAlphaF(_params.alpha);
    QImage* mask_image = getIntegrationMask(_region_data->mask(frame_index), peak_color, bkg_color);
    QGraphicsPixmapItem* mask = view->scene()->addPixmap(QPixmap::fromImage(*mask_image));
    mask->setZValue(-1);
}

QImage* PeakWindow::getIntegrationMask(const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg)
{
    QImage* region_img = new QImage(mask.cols(), mask.rows(), QImage::Format_ARGB32);

    for (int c = 0; c < mask.cols(); ++c) {
        for (int r = 0; r < mask.rows(); ++r) {
            nsx::IntegrationRegion::EventType ev = nsx::IntegrationRegion::EventType(mask(r, c));
            QColor color;

            switch (ev) {
                case nsx::IntegrationRegion::EventType::PEAK: color = peak; break;
                case nsx::IntegrationRegion::EventType::BACKGROUND: color = bkg; break;
                default: color = Qt::transparent; break;
            }

            // todo: what about unselected peaks?
            region_img->setPixelColor(QPoint(c, r), color);
        }
    }
    return region_img;
}

void PeakWindow::setLabel()
{
    QString text;
    QTextStream(&text) << "hkl: (" << _integration_region->peak()->hkl().h() << ", "
                       << _integration_region->peak()->hkl().k() << ", "
                       << _integration_region->peak()->hkl().l() << ")   position: ("
                       << _integration_region->peak()->shape().center()[0] << ", "
                       << _integration_region->peak()->shape().center()[1] << ", "
                       << _integration_region->peak()->shape().center()[2]
                       << ")   I = " << _integration_region->peak()->correctedIntensity().value()
                       << "   " << QString(QChar(0x03C3)) << " = "
                       << _integration_region->peak()->correctedIntensity().sigma();
    QLineEdit* line = new QLineEdit();
    line->setText(text);
    line->setReadOnly(true);
    _grid_layout->addWidget(line, 1, 0, 1, _region_data->nFrames());
}

void PeakWindow::closeEvent(QCloseEvent* event)
{
    remove();
    QDialog::closeEvent(event);
    delete this;
}

void PeakWindow::remove()
{
    // Remove the pointer from vector owned by MainWin
    for (std::size_t i = 0; i <= gGui->peak_windows.size(); ++i) {
        if (gGui->peak_windows[i] == this) {
            gGui->peak_windows.remove(i);
            break;
        }
    }
}

QSize PeakWindow::sizeHint() const
{
    double w = gGui->sizeHint().width();
    double h = QDialog::sizeHint().height();
    return QSize(w, h);
}

void PeakWindow::grabParameters()
{
    _peak_end->setValue(_params.peak_end);
    _bkg_begin->setValue(_params.bkg_begin);
    _bkg_end->setValue(_params.bkg_end);
    _alpha->setValue(_params.alpha);
    _peak_color_button->setColor(_params.peak_color);
    _bkg_color_button->setColor(_params.bkg_color);
    _intensity_slider->setValue(_params.max_intensity);
}

void PeakWindow::setParameters()
{
    _params.peak_end = _peak_end->value();
    _params.bkg_begin = _bkg_begin->value();
    _params.bkg_end = _bkg_end->value();
    _params.peak_color = _peak_color_button->color();
    _params.bkg_color = _bkg_color_button->color();
    _params.alpha = _alpha->value();
    _params.max_intensity = _intensity_slider->value();
}
