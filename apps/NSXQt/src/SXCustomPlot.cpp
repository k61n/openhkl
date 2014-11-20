#include "include/SXCustomPlot.h"
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>

SXCustomPlot::SXCustomPlot(QWidget *parent) :
    QCustomPlot(parent)
{
}

void SXCustomPlot::copyViewToClipboard()
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

void SXCustomPlot::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
    {
        copyViewToClipboard();
    }
}

void SXCustomPlot::mousePress(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(yAxis->orientation());
    else
        axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void SXCustomPlot::mouseWheel(QWheelEvent* event)
{
    Q_UNUSED(event);
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(yAxis->orientation());
    else
        axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

SXCustomPlot::~SXCustomPlot()
{
}
