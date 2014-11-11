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
