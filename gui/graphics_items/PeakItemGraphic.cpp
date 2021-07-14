//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PeakItemGraphic.cpp
//! @brief     Implements class PeakItemGraphic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/PeakItemGraphic.h"

#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Units.h"

#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/IntegrationRegion.h"
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"

#include "gui/graphics/PeakPlot.h"
#include "gui/utility/ColorButton.h"

#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

PeakItemGraphic::PeakItemGraphic(nsx::Peak3D* peak)
    : PlottableItem(nullptr, true, false), _peak(peak)
{
    setVisible(true);
    _size = Eigen::Vector2d(10, 10);
    _center_color = Qt::red;
    _show_label = false;
    _show_center = true;
    redraw();
}

void PeakItemGraphic::redraw()
{
    QString peak_label;
    const nsx::UnitCell* unit_cell = _peak->unitCell();
    if (unit_cell) {
        nsx::MillerIndex hkl = _peak->hkl();
        if (hkl.indexed(unit_cell->indexingTolerance())) {
            peak_label = QString("%1,%2,%3").arg(hkl.h()).arg(hkl.k()).arg(hkl.l());
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
    _label_gi->setPos(3, 3);
    _label_gi->setVisible(_show_label);

    _center_gi = new QGraphicsEllipseItem(this);
    _center_gi->setRect(-_size[0] / 2, -_size[1] / 2, _size[0], _size[1]);
    _center_gi->setParentItem(this);
    _center_gi->setBrush(QBrush(_center_color));
    _center_gi->setAcceptHoverEvents(false);
    _center_gi->setZValue(10);
    _center_gi->setVisible(_show_center);

    nsx::Ellipsoid peak_ellipsoid = _peak->shape();
    peak_ellipsoid.scale(_peak->peakEnd());
    const nsx::AABB& aabb = peak_ellipsoid.aabb();
    _lower = aabb.lower();
    _upper = aabb.upper();

    setBoundingRegionGranularity(0.0);

    if (peak()->enabled()) {
        setColor(Qt::darkGreen);
        setCenterColor(Qt::darkGreen);
    } else {
        setColor(Qt::red);
        setCenterColor(Qt::red);
    }

    // A peak item is always put on foreground of the scene
    setZValue(2);
}

void PeakItemGraphic::setCenter(int frame)
{
    nsx::Ellipsoid peak_ellipsoid = _peak->shape();

    Eigen::Vector3d center =
        peak_ellipsoid.intersectionCenter({0.0, 0.0, 1.0}, {0.0, 0.0, static_cast<double>(frame)});

    setPos(center[0], center[1]);
}

void PeakItemGraphic::setSize(int size)
{
    _size = Eigen::Vector2d(size, size);
    _center_gi->setRect(-_size[0] / 2, -_size[1] / 2, _size[0], _size[1]);
}

void PeakItemGraphic::setColor(QColor color)
{
    _center_color = color;
    _center_gi->setBrush(QBrush(_center_color));
}

void PeakItemGraphic::setCenterColor(QColor color)
{
    _center_color = color;
    QPen center_pen;
    center_pen.setCosmetic(true);
    center_pen.setColor(_center_color);
    center_pen.setStyle(Qt::SolidLine);
    _center_gi->setPen(center_pen);
}

void PeakItemGraphic::initFromPeakViewWidget(const PeakViewWidget::Set& set)
{
    showArea(set.drawPeaks->isChecked());
    setSize(set.sizePeaks->value());
    setCenterColor(set.colorPeaks->color());
}

nsx::Peak3D* PeakItemGraphic::peak() const
{
    return _peak;
}

QRectF PeakItemGraphic::boundingRect() const
{
    double width = _upper[0] - _lower[0]; // determined from AABB in constructor
    double height = _upper[1] - _lower[1];
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

void PeakItemGraphic::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)

    Q_UNUSED(option)

    Q_UNUSED(painter)

    // _label_gi->setVisible(_hovered || _show_label);
    // _center_gi->setVisible(_hovered || _show_center);
}

std::string PeakItemGraphic::getPlotType() const
{
    return "peak";
}

void PeakItemGraphic::showLabel(bool flag)
{
    _show_label = flag;
    _label_gi->setVisible(_show_label);
}

void PeakItemGraphic::showArea(bool flag)
{
    _show_center = flag;
    _center_gi->setVisible(_show_center);
}

void PeakItemGraphic::plot(SXPlot* plot)
{
    PeakPlot* p = dynamic_cast<PeakPlot*>(plot);
    if (!p)
        return;

    const std::vector<nsx::Intensity>& rockingCurve = _peak->rockingCurve();
    const int N = int(rockingCurve.size());

    // Transform to QDouble
    QVector<double> q_frames(N);
    QVector<double> q_intensity(N);
    QVector<double> q_error(N);

    // Copy the data
    const double center = std::round(_peak->shape().center()(2));

    for (int i = 0; i < N; ++i) {
        q_frames[i] = center - i / 2.0;
        q_intensity[i] = rockingCurve[i].value();
        q_error[i] = rockingCurve[i].sigma();
    }
    p->graph(0)->setData(q_frames, q_intensity);
    p->addErrorBars(p->graph(0), q_error);

    // Now update text info:
    QString info;

    if (const nsx::UnitCell* cell = _peak->unitCell()) {
        nsx::MillerIndex hkl = _peak->hkl();
        if (hkl.indexed(cell->indexingTolerance())) {
            info = "(h,k,l):" + QString::number(hkl.h()) + "," + QString::number(hkl.k()) + ","
                + QString::number(hkl.l());
        } else {
            info = "unindexed";
        }
    } else {
        info = "no unit cell";
    }

    const Eigen::Vector3d c = _peak->shape().center();
    const nsx::InterpolatedState state = _peak->dataSet()->instrumentStates().interpolate(c[2]);
    const nsx::DirectVector position =
        _peak->dataSet()->reader()->diffractometer()->detector()->pixelPosition(c[0], c[1]);
    const double g = state.gamma(position) / nsx::deg;
    const double n = state.nu(position) / nsx::deg;
    info += " " + QString(QChar(0x03B3)) + "," + QString(QChar(0x03BD)) + ":"
        + QString::number(g, 'f', 2) + "," + QString::number(n, 'f', 2) + "\n";
    const nsx::Intensity corr_int = _peak->correctedIntensity();
    const double intensity = corr_int.value();
    const double sI = corr_int.sigma();
    info += "Intensity (" + QString(QChar(0x03C3)) + "I): " + QString::number(intensity) + " ("
        + QString::number(sI, 'f', 2) + ")\n";
    info += "Cor. int. (" + QString(QChar(0x03C3)) + "I): " + QString::number(intensity, 'f', 2)
        + " (" + QString::number(sI, 'f', 2) + ")\n";

    const double scale = _peak->scale();
    const double monitor = _peak->dataSet()->reader()->metadata().key<double>(nsx::at_monitorSum);
    info += "Monitor " + QString::number(monitor * scale) + " counts";
    QCPTextElement* title = dynamic_cast<QCPTextElement*>(p->plotLayout()->element(0, 0));
    if (title != nullptr)
        title->setText(info);
    p->rescaleAxes();
    p->replot();
}
