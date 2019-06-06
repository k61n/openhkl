
#include "gui/graphics/cutlineitem.h"
#include "gui/graphics/simpleplot.h"
#include "gui/graphics/detectorscene.h"
#include <Eigen/Dense>

CutLineItem::CutLineItem(nsx::sptrDataSet data)
    : CutterItem(data), _nPoints(10)
{}

CutLineItem::~CutLineItem() {}

void CutLineItem::plot(NSXPlot* plot)
{
    auto p = dynamic_cast<SimplePlot*>(plot);
    if (!p) {
        return;
    }
    p->xAxis->setLabel("Interpolation point");
    p->yAxis->setLabel("Intensity (counts)");

    // Set the pointer to the detector scene to the scene that holds the cutter
    auto detPtr = dynamic_cast<DetectorScene*>(scene());
    if (!detPtr) {
        return;
    }
    QVector<double> x(_nPoints);
    QVector<double> y(_nPoints);
    QVector<double> e(_nPoints);

    QLineF line;
    line.setP1(sceneBoundingRect().bottomLeft());
    line.setP2(sceneBoundingRect().topRight());
    const Eigen::MatrixXi& currentFrame = detPtr->getCurrentFrame();
    std::iota(x.begin(), x.end(), 0);

    for (int i = 0; i < _nPoints; ++i) {

        QPointF point = line.pointAt(i / static_cast<double>(_nPoints));
        int ipx = static_cast<int>(point.x());
        int ipy = static_cast<int>(point.y());
        QPoint lowestCorner = QPoint(ipx, ipy);
        double sdist2 = 0.0;

        for (int pi = 0; pi < 2; ++pi) {
            for (int pj = 0; pj < 2; ++pj) {
                QPoint currentCorner = lowestCorner + QPoint(pi, pj);
                QPointF dp = point - currentCorner;
                double dist2 = dp.x() * dp.x() + dp.y() * dp.y();
                // bugfix? x and y should be swapped here
                // int count=currentFrame(currentCorner.x(),currentCorner.y());
                int count = currentFrame(currentCorner.y(), currentCorner.x());
                y[i] += dist2 * count;
                sdist2 += dist2;
            }
        }
        y[i] /= sdist2;
    }
    std::transform(y.begin(), y.end(), e.begin(), [](double p) { return sqrt(p); });
    p->graph(0)->setDataValueError(x, y, e);
    p->rescaleAxes();
    p->replot();
}

void CutLineItem::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected) {
        _pen.setStyle(Qt::DashLine);
    } else {
        _pen.setStyle(Qt::SolidLine);
    }
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setPen(_pen);
    qreal w = _to.x() - _from.x();
    qreal h = _to.y() - _from.y();
    painter->drawLine(-w / 2, -h / 2, w / 2, h / 2);
}

int CutLineItem::getNPoints() const
{
    return _nPoints;
}

void CutLineItem::setNPoints(int nPoints)
{
    _nPoints = nPoints;
}

void CutLineItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!isVisible()) {
        return;
    }
    if (!isSelected()) {
        return;
    }
    _nPoints += event->delta() > 0 ? 1 : -1;
    if (_nPoints <= 0) {
        _nPoints = 1;
    }
}

std::string CutLineItem::getPlotType() const
{
    return "simple";
}
