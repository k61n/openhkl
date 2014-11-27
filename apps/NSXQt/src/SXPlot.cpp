#include "SXPlot.h"
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>

SXPlot::SXPlot(QWidget *parent) : QCustomPlot(parent)
{
}

void SXPlot::update(PlottableGraphicsItem *item)
{
    Q_UNUSED(item)
}

std::string SXPlot::getType() const
{
    return "";
}

void SXPlot::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(this->rect().size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    this->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
}

void SXPlot::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
        copyViewToClipboard();
}

void SXPlot::mousePress(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(yAxis->orientation());
    else
        axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void SXPlot::mouseWheel(QWheelEvent* event)
{
    Q_UNUSED(event);
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(yAxis->orientation());
    else
        axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

SXPlot::~SXPlot()
{
}
