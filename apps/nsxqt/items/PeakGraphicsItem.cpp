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

#include "PeakGraphicsItem.h"
#include "PeakPlot.h"
#include "SXPlot.h"


bool PeakGraphicsItem::_labelVisible = false;
bool PeakGraphicsItem::_drawBackground = false;

PeakGraphicsItem::PeakGraphicsItem(nsx::sptrPeak3D p):
    PlottableGraphicsItem(nullptr,true,false),
    _peak(std::move(p))
{ 
    if (_peak) {
        Eigen::Vector3d c=_peak->getIntegrationRegion().aabb().center();
        setPos(c[0], c[1]);
    }
    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _pen.setColor(QColor(0,0,255,255));

    QString hkl;
    _label=new QGraphicsTextItem(this);
    //Ensure text is alwyas real size despite zoom
    _label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label->setParentItem(this);
    setBoundingRegionGranularity(0.0);

    // A peak item is always put on foreground of the scene
    setZValue(2);
}

QRectF PeakGraphicsItem::boundingRect() const
{
    auto aabb = _peak->getIntegrationRegion().aabb();
    const Eigen::Vector3d& l = aabb.lower();
    const Eigen::Vector3d& u = aabb.upper();
    qreal w=u[0]-l[0];
    qreal h=u[1]-l[1];

    // currently debugging...

    if (w < 0.0) {
        //qDebug() << "width is less than zero!";
        w = 0.0;
    }

    if (h < 0.0) {
        //qDebug() << "height is less than zero!";
        h = 0.0;
    }

    return QRectF(-w/2.0,-h/2.0,w,h);
}

void PeakGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (!isVisible()) {
        return;
    }
    if (option->state & QStyle::State_Selected) {
        _pen.setStyle(Qt::DotLine);
    } else {
        _pen.setStyle(Qt::SolidLine);
    }
    painter->setRenderHint(QPainter::Antialiasing);

    if (_hovered) {
        painter->setBrush(QBrush(QColor(255,255,0,120)));
    }
    _label->setVisible(_hovered || _labelVisible);

    const auto aabb = _peak->getIntegrationRegion().aabb();
    const Eigen::Vector3d& peak_l = aabb.lower();
    const Eigen::Vector3d& peak_u = aabb.upper();
    qreal peak_w = peak_u[0]-peak_l[0];
    qreal peak_h = peak_u[1]-peak_l[1];

    _label->setPos(peak_w/2,peak_h/2);
}

void PeakGraphicsItem::setFrame(unsigned long frame)
{
    const auto aabb = _peak->getIntegrationRegion().aabb();
    const Eigen::Vector3d& l = aabb.lower();
    const Eigen::Vector3d& u = aabb.upper();

    // out of bounds
    if (frame < l[2] || frame > u[2]) {
        setVisible(false);
        _label->setVisible(false);
        return;
    }

    setVisible(true);
    _label->setVisible(_labelVisible);
    QString hklString;

    if (auto cell = _peak->activeUnitCell()) {
        nsx::MillerIndex miller_index(_peak,cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            hklString = QString("%1,%2,%3").arg(miller_index[0]).arg(miller_index[1]).arg(miller_index[2]);
        } else {
            hklString = "unindexed";
        }
    } else {
        hklString = "no unit cell";
    }
    _label->setPlainText(hklString);
}

std::string PeakGraphicsItem::getPlotType() const
{
    return "peak";
}

nsx::sptrPeak3D PeakGraphicsItem::getPeak()
{
    return _peak;
}

void PeakGraphicsItem::setLabelVisible(bool flag)
{
    _labelVisible = flag;
}

void PeakGraphicsItem::drawBackground(bool flag)
{
    _drawBackground = flag;
}

void PeakGraphicsItem::plot(SXPlot* plot)
{

    auto p=dynamic_cast<PeakPlot*>(plot);
    if (p == nullptr) {
        return;
    }

    const Eigen::VectorXd& total=_peak->getProjection();
    const Eigen::VectorXd& signal=_peak->getPeakProjection();
    const Eigen::VectorXd& bkg=_peak->getBkgProjection();

    // Transform to QDouble
    QVector<double> qx(int(total.size()));
    QVector<double> qtotal(int(total.size()));
    QVector<double> qtotalE(int(total.size()));
    QVector<double> qpeak(int(total.size()));
    QVector<double> qbkg(int(total.size()));

    //Copy the data
    nsx::Ellipsoid background = _peak->getShape();
    const auto aabb = background.aabb();
    background.scale(3.0);
    double min=std::floor(aabb.lower()[2]);
    double max=std::ceil(aabb.upper()[2]);

    if (min<0) {
        min=0;
    }
    if (max>_peak->data()->nFrames()-1) {
        max=_peak->data()->nFrames()-1;
    }

    Eigen::VectorXd error = _peak->getIntegration().getPeakError();

    for (int i = 0; i < total.size(); ++i) {
        qx[i]= min + static_cast<double>(i)*(max-min)/(total.size()-1);
        qtotal[i]=total[i];
        qtotalE[i]=error[i];
        qpeak[i]=signal[i];
        qbkg[i]=bkg[i];
    }
    p->graph(0)->setDataValueError(qx, qtotal, qtotalE);
    p->graph(1)->setData(qx,qpeak);
    p->graph(2)->setData(qx,qbkg);

    // Now update text info:
    Eigen::RowVector3d hkl;
    QString info;

    if (auto cell = _peak->activeUnitCell()) {
        nsx::MillerIndex miller_index(_peak,cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            info="(h,k,l):"+QString::number(miller_index[0])+","+QString::number(miller_index[1])+","+QString::number(miller_index[2]);
        } else {
            info = "unindexed";
        }
    } else {
        info = "no unit cell";
    }

    auto c = _peak->getShape().center();
    auto state = _peak->data()->interpolatedState(c[2]);
    auto position = _peak->data()->diffractometer()->getDetector()->pixelPosition(c[0], c[1]);
    double g = state.gamma(position);
    double n = state.nu(position);
    g/=nsx::deg;
    n/=nsx::deg;
    info+=" "+QString(QChar(0x03B3))+","+QString(QChar(0x03BD))+":"+QString::number(g,'f',2)+","+QString::number(n,'f',2)+"\n";
    double intensity=_peak->getScaledIntensity().value();
    double sI=_peak->getScaledIntensity().sigma();
    info+="Intensity ("+QString(QChar(0x03C3))+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
    double l = state.lorentzFactor(position);
    info+="Cor. int. ("+QString(QChar(0x03C3))+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";
    info += "p value (" + QString::number(_peak->pValue(), 'f', 3) + ")\n";

    double scale=_peak->getScale();
    double monitor=_peak->data()->metadata()->getKey<double>("monitor");
    info+="Monitor "+QString::number(monitor*scale)+" counts";
    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(p->plotLayout()->element(0,0));
    if (title != nullptr) {
        title->setText(info);
    }
    p->rescaleAxes();
    p->replot();
}
