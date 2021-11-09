//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include "core/peak/IntegrationRegion.h"
#include "gui/models/ColorMap.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

PeakWindow::PeakWindow(nsx::IntegrationRegion* region, QWidget* parent)
    : QDialog(parent)
    , _integration_region(region)
    , _intensity(3000)
    , _logarithmic(false)
    , _colormap(new ColorMap())
    , _image(nullptr)
    , _peak_color(QColor(0, 255, 0, 32)) // green, alpha = 1/8
    , _bkg_color(QColor(255, 255, 0, 32)) // yellow, alpha = 1/8
{
    setModal(false);

    QWidget* _preview_widget = new QWidget(this);
    QWidget* _control_widget = new QWidget(this);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QHBoxLayout* view_layout = new QHBoxLayout(_preview_widget);
    QHBoxLayout* control_layout = new QHBoxLayout();

    setGraphicsViewUp();

    view_layout->addWidget(_preview_widget);
    control_layout->addWidget(_control_widget);
    main_layout->addLayout(view_layout);
    main_layout->addLayout(control_layout);
}

void PeakWindow::setGraphicsViewUp()
{
    _graphics_view = new QGraphicsView;
    _frame_slider = new QSlider(_preview_widget);

    _frame_slider->setOrientation(Qt::Horizontal);
    _frame_slider->setMinimum(1);
    _frame_slider->setValue(1);

    QVBoxLayout* graphics = new QVBoxLayout;
    graphics->addWidget(_graphics_view);
    graphics->addWidget(_frame_slider);
}

void PeakWindow::drawFrame(std::size_t frame_index)
{
    _region_data = _integration_region->getRegion();

    QRect rect(0, 0, _region_data.xmax(), _region_data.ymax());
    if (!_graphics_view->scene())
        _graphics_view->setScene(new QGraphicsScene());
    _graphics_view->scene()->clear(); // clear the scene
    _graphics_view->scene()->setSceneRect(rect);


    // add the image data
    if (_image == nullptr) {
        _image = _graphics_view->scene()->addPixmap(
            QPixmap::fromImage(
                _colormap->matToImage(_region_data.frame(frame_index).cast<double>(), rect,
                                      _intensity, _logarithmic)));
        _image->setZValue(-2);
    } else {
        _image->setPixmap(
            QPixmap::fromImage(_colormap->matToImage(_region_data.frame(frame_index).cast<double>(),
                                                     rect, _intensity, _logarithmic)));
    }

    // add the integration overlay
    QImage* mask_image = getIntegrationMask(_region_data.mask(frame_index), _peak_color, _bkg_color);
    if (_integration_overlay)
        _integration_overlay->setPixmap(QPixmap::fromImage(*mask_image));
    else
        _integration_overlay = _graphics_view->scene()->addPixmap(QPixmap::fromImage(*mask_image));
    _integration_overlay->setZValue(-1);

    _graphics_view->fitInView(0, 0, _region_data.xmax(), _region_data.ymax());
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

void PeakWindow::refreshAll()
{
}
