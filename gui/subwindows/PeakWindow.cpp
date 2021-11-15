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
#include "gui/MainWin.h" // gGui

#include <QVBoxLayout>
#include <QPixmap>

PeakWindow::PeakWindow(QWidget* parent, nsx::IntegrationRegion* region)
    : QDialog(parent)
    , _integration_region(region)
    , _intensity(3000)
    , _logarithmic(false)
    , _colormap(new ColorMap())
    , _peak_color(QColor(0, 255, 0, 32)) // green, alpha = 1/8
    , _bkg_color(QColor(255, 255, 0, 32)) // yellow, alpha = 1/8
{
    setModal(false);
    _main_layout = new QGridLayout(this);
    gGui->peak_windows.push_back(this);
    if (region)
        _region_data = _integration_region->getRegion();
}

void PeakWindow::refreshAll()
{
    for (auto* view : _views)
        delete view;
    _views.clear();
    for (std::size_t i = 0; i < _region_data->nFrames(); ++i) {
        QGraphicsView* view = drawFrame(i);
        _views.push_back(view);
        _main_layout->addWidget(view, 0, i, 1, 1);
    }
}

void PeakWindow::setIntegrationRegion(nsx::IntegrationRegion* region)
{
    _integration_region = region;
    _region_data = _integration_region->getRegion();
}

QGraphicsView* PeakWindow::drawFrame(std::size_t frame_index)
{
    if (!_integration_region)
        return nullptr;

    QGraphicsView* view = new QGraphicsView();
    QRect rect(0, 0, _region_data->cols()+1, _region_data->rows()+1);
    if (!view->scene())
        view->setScene(new QGraphicsScene());
    view->scene()->clear(); // clear the scene
    view->scene()->setSceneRect(rect);


    // add the image data
    QGraphicsPixmapItem* image = view->scene()->addPixmap(
        QPixmap::fromImage(
            _colormap->matToImage(_region_data->frame(frame_index).cast<double>(), rect,
                                    _intensity, _logarithmic)));
    image->setZValue(-2);

    // add the integration overlay
    QImage* mask_image = getIntegrationMask(_region_data->mask(frame_index), _peak_color, _bkg_color);
    QGraphicsPixmapItem* mask = view->scene()->addPixmap(QPixmap::fromImage(*mask_image));
    mask->setZValue(-1);

    view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff );
    view->scale(0.25, 0.25);
    return view;
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

QSize PeakWindow::sizeHint() const
{
    QSize hint = QDialog::sizeHint();
    hint.setWidth(gGui->sizeHint().rwidth());
    return hint;
}

void PeakWindow::closeEvent(QCloseEvent* event)
{
    // Remove the pointer from vector owned by MainWin
    for (std::size_t i = 0; i <= gGui->peak_windows.size(); ++i) {
        if (gGui->peak_windows[i] == this) {
            gGui->peak_windows.remove(i);
            break;
        }
    }
    QDialog::closeEvent(event);
    delete this;
}
