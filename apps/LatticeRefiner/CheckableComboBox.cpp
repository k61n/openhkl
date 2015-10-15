#include <QAbstractItemView>
#include <QEvent>
#include <QLineEdit>
#include <QStylePainter>
#include <QDebug>

#include "CheckableComboBox.h"
#include "CheckableComboBoxDelegate.h"

CheckableComboBox::CheckableComboBox(QWidget *parent) : QComboBox(parent)
{
    setFixedSize(110,27);
}

CheckableComboBox::~CheckableComboBox()
{
}

void CheckableComboBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox opt;
    initStyleOption(&opt);

    opt.currentText = "Constraints";

    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
