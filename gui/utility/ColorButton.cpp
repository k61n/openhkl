

#include "gui/utility/ColorButton.h"

#include <QColorDialog>

ColorButton::ColorButton(const QColor& color, QWidget* parent) : QPushButton(parent)
{
    this->setMinimumWidth(50);
    currentColor = color;
    connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

QColor ColorButton::getColor()
{
    return currentColor;
}

void ColorButton::changeColor(const QColor& color)
{
    currentColor = color;
    colorChanged(currentColor);
}

void ColorButton::chooseColor()
{
    QColor color = QColorDialog::getColor(currentColor, this);
    if (color.isValid())
        changeColor(color);
}

void ColorButton::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);

    int colorPadding = 5;

    QPainter painter(this);
    painter.setBrush(QBrush(currentColor));
    painter.setPen("#CECECE");
    painter.drawRect(
        rect().adjusted(colorPadding, colorPadding, -1 - colorPadding, -1 - colorPadding));
}