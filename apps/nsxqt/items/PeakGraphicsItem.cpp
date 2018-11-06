#include <iostream>

#include <Eigen/Dense>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include <nsxlib/DataSet.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
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

void PeakGraphicsItem::showArea(bool flag)
{
    _show_center = flag;
}

void PeakGraphicsItem::plot(SXPlot* plot)
{

    auto p = dynamic_cast<PeakPlot*>(plot);
    if (!p) {
        return;
    }

    const auto& rockingCurve =_peak->rockingCurve();
    const int N = int(rockingCurve.size());

    // Transform to QDouble
    QVector<double> q_frames(N);
    QVector<double> q_intensity(N);
    QVector<double> q_error(N);

    // Copy the data
    double center = std::round(_peak->shape().center()(2));
    
    for (int i = 0; i < N; ++i) {
        q_frames[i]= center - i/2.0;
        q_intensity[i] = rockingCurve[i].value();
        q_error[i] = rockingCurve[i].sigma();
    }
    p->graph(0)->setDataValueError(q_frames, q_intensity, q_error);

    // Now update text info:
    QString info;

    if (auto cell = _peak->unitCell()) {
        nsx::MillerIndex miller_index(_peak->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            info="(h,k,l):"+QString::number(miller_index[0])+","+QString::number(miller_index[1])+","+QString::number(miller_index[2]);
        } else {
            info = "unindexed";
        }
    } else {
        info = "no unit cell";
    }

    auto c = _peak->shape().center();
    auto state = _peak->data()->interpolatedState(c[2]);
    auto position = _peak->data()->diffractometer()->detector()->pixelPosition(c[0], c[1]);
    double g = state.gamma(position);
    double n = state.nu(position);
    g/=nsx::deg;
    n/=nsx::deg;
    info+=" "+QString(QChar(0x03B3))+","+QString(QChar(0x03BD))+":"+QString::number(g,'f',2)+","+QString::number(n,'f',2)+"\n";
    double intensity=_peak->correctedIntensity().value();
    auto corr_int = _peak->correctedIntensity();
    double sI=_peak->correctedIntensity().sigma();
    info+="Intensity ("+QString(QChar(0x03C3))+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";  
    info+="Cor. int. ("+QString(QChar(0x03C3))+"I): "+QString::number(corr_int.value(),'f',2)+" ("+QString::number(corr_int.sigma(),'f',2)+")\n";

    double scale=_peak->scale();
    double monitor=_peak->data()->metadata()->key<double>("monitor");
    info+="Monitor "+QString::number(monitor*scale)+" counts";
    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(p->plotLayout()->element(0,0));
    if (title != nullptr) {
        title->setText(info);
    }
    p->rescaleAxes();
    p->replot();
}
