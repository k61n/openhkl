//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/CutSliceItem.cpp
//! @brief     Implements class CutSliceItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_tools/CutSliceItem.h"

#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/SimplePlot.h"
#include <Eigen/Dense>

CutSliceItem::CutSliceItem(ohkl::sptrDataSet data, bool horizontal)
    : CutterItem(data), _horizontal(horizontal)
{
}

void CutSliceItem::plot(SXPlot* plot)
{
    SimplePlot* p = dynamic_cast<SimplePlot*>(plot);
    if (!p)
        return;
    p->xAxis->setLabel("Frame (a.u.)");
    p->yAxis->setLabel("Intensity (counts)");

    // Set the pointer to the detector scene to the scene that holds the cutter
    DetectorScene* detPtr = dynamic_cast<DetectorScene*>(scene());
    if (!detPtr)
        return;

    ohkl::sptrDataSet data = detPtr->getData();
    const ohkl::Detector* det = data->diffractometer()->detector();

    int nrows = det->nRows();
    int ncols = det->nCols();

    // Define the position on the scene of the cutter
    int xmin = sceneBoundingRect().left();
    int xmax = sceneBoundingRect().right();
    int ymin = sceneBoundingRect().top();
    int ymax = sceneBoundingRect().bottom();

    xmin = std::max(xmin, 0);
    xmax = std::min(xmax, ncols);
    ymin = std::max(ymin, 0);
    ymax = std::min(ymax, nrows);

    int length;
    int start;

    bool horizontal = isHorizontal();

    int dx = xmax - xmin;
    int dy = ymax - ymin;

    if (horizontal) {
        length = dx;
        start = xmin;
    } else {
        length = dy;
        start = ymin;
    }
    QVector<double> x(length);
    QVector<double> y(length);
    QVector<double> e(length);
    std::iota(x.begin(), x.end(), start);
    const RowMatrix& currentFrame = detPtr->currentFrameMatrix();

    if (horizontal) {
        int comp = 0;
        for (int i = xmin; i < xmax; ++i)
            y[comp++] = currentFrame.col(i).segment(ymin, dy).sum();
    } else {
        int comp = 0;
        for (int i = ymin; i < ymax; ++i)
            y[comp++] = currentFrame.row(i).segment(xmin, dx).sum();
    }
    std::transform(y.begin(), y.end(), e.begin(), [](double p) { return sqrt(p); });
    p->graph(0)->setData(x, y);
    p->addErrorBars(p->graph(0), e);
    p->rescaleAxes();
    p->replot(QCustomPlot::rpRefreshHint);
}

bool CutSliceItem::isHorizontal() const
{
    return _horizontal;
}

void CutSliceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    if (_horizontal)
        painter->setBrush(QBrush(QColor(0, 255, 0, 50)));
    else
        painter->setBrush(QBrush(QColor(0, 0, 255, 50)));
    // Color depending on selection
    if (option->state & QStyle::State_Selected)
        _pen.setStyle(Qt::DashLine);
    else
        _pen.setStyle(Qt::SolidLine);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(_pen);
    qreal w = std::abs(_to.x() - _from.x());
    qreal h = std::abs(_to.y() - _from.y());
    painter->drawRect(-w / 2.0, -h / 2.0, w, h);
}

std::string CutSliceItem::getPlotType() const
{
    return "simple";
}

void CutSliceItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!isVisible())
        return;
    if (!isSelected())
        return;
    int step = event->delta() / 120;

    if (_horizontal) {
        _from += QPointF(0, -step);
        _to += QPointF(0, step);
    } else {
        _from += QPointF(-step, 0);
        _to += QPointF(step, 0);
    }
    update();
}
