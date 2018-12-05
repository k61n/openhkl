#include <iostream>

#include <Eigen/Dense>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include <nsxlib/DataSet.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/IntegrationRegion.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "DetectorScene.h"
#include "PeakGraphicsItem.h"
#include "PeakPlot.h"
#include "SXPlot.h"

bool PeakGraphicsItem::_show_label = false;
bool PeakGraphicsItem::_show_center = false;

PeakGraphicsItem::PeakGraphicsItem(nsx::sptrPeak3D peak, int frame)
: PlottableGraphicsItem(nullptr,true,false),
  _peak(peak),
  _area(nullptr)
{
    setVisible(true);

    QString peak_label;
    auto unit_cell = _peak->unitCell();
    if (unit_cell) {
        nsx::MillerIndex miller_index(_peak->q(), *unit_cell);
        if (miller_index.indexed(unit_cell->indexingTolerance())) {
            peak_label = QString("%1,%2,%3").arg(miller_index[0]).arg(miller_index[1]).arg(miller_index[2]);
        } else {
            peak_label = "not indexed";
        }

    } else {
        peak_label = "no unit cell";
    }

    _label_gi = new QGraphicsTextItem(this);
    _label_gi->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label_gi->setParentItem(this);
    _label_gi->setPlainText(peak_label);
    _label_gi->setAcceptHoverEvents(false);
    _label_gi->setZValue(-1);
    _label_gi->setPos(3,3);
    _label_gi->setVisible(_show_label);

    QPen center_pen;
    center_pen.setCosmetic(true);
    center_pen.setColor(Qt::red);
    center_pen.setStyle(Qt::SolidLine);

    _center_gi = new QGraphicsEllipseItem(this);
    _center_gi->setPen(center_pen);
    _center_gi->setRect(-1,-1,2,2);
    _center_gi->setParentItem(this);
    _center_gi->setBrush(QBrush(Qt::red));
    _center_gi->setAcceptHoverEvents(false);
    _center_gi->setZValue(-1);
    _center_gi->setVisible(_show_center);

    auto peak_ellipsoid = _peak->shape();

    peak_ellipsoid.scale(_peak->peakEnd());

    auto& aabb = peak_ellipsoid.aabb();

    _lower = aabb.lower();

    _upper = aabb.upper();

    auto center = peak_ellipsoid.intersectionCenter({0.0,0.0,1.0},{0.0,0.0,static_cast<double>(frame)});

    setPos(center[0],center[1]);

    setBoundingRegionGranularity(0.0);

    // A peak item is always put on foreground of the scene
    setZValue(2);
}

nsx::sptrPeak3D PeakGraphicsItem::peak() const
{
    return _peak;
}

QRectF PeakGraphicsItem::boundingRect() const
{

    double width = _upper[0] - _lower[0];

    double height = _upper[1] - _lower[1];

    return QRectF(-width/2.0,-height/2.0,width,height);
}

void PeakGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)

    Q_UNUSED(option)

    Q_UNUSED(painter)

    _label_gi->setVisible(_hovered || _show_label);

    _center_gi->setVisible(_hovered || _show_center);
}

std::string PeakGraphicsItem::getPlotType() const
{
    return "peak";
}

void PeakGraphicsItem::showLabel(bool flag)
{
    _show_label = flag;
}

void PeakGraphicsItem::showCenter(bool flag)
{
    _show_center = flag;
}

void PeakGraphicsItem::plot(SXPlot* plot)
{
    auto p = dynamic_cast<PeakPlot*>(plot);
    if (!p) {
        return;
    }

    auto ellipsoid = _peak->shape();
    ellipsoid.scale(_peak->bkgEnd());
    const auto& aabb = ellipsoid.aabb();
    Eigen::Vector3i lower = aabb.lower().cast<int>();

    lower[0] = (lower[0] < 0) ? 0 : lower[0];
    lower[1] = (lower[1] < 0) ? 0 : lower[1];
    lower[2] = (lower[2] < 0) ? 0 : lower[2];

    auto data = _peak->data();
    const int n_rows = data->nRows();
    const int n_cols = data->nCols();
    const int n_frames = data->nFrames();

    Eigen::Vector3i upper = aabb.upper().cast<int>();
    upper[0] = (upper[0] >= n_cols) ? n_cols - 1 : upper[0];
    upper[1] = (upper[1] >= n_rows) ? n_rows - 1 : upper[1];
    upper[2] = (upper[2] >= n_frames) ? n_frames - 1 : upper[2];

    QVector<double> x_values;
    QVector<double> y_values;
    QVector<double> err_y_values;

    for (int z = lower[2]; z <= upper[2]; ++z) {
        const auto& frame = data->frame(z);
        double counts = static_cast<double>(frame.block(lower[1],lower[0],upper[1]-lower[1],upper[0]-lower[0]).sum());
        x_values.append(static_cast<double>(z));
        y_values.append(counts);
        err_y_values.append(counts > 0 ? std::sqrt(counts) : 0.0);
    }

    p->graph(0)->setDataValueError(x_values, y_values, err_y_values);

    p->xAxis->setAutoTicks(false);
    p->xAxis->setTickVector(x_values);

    p->rescaleAxes();
    p->replot();
}
