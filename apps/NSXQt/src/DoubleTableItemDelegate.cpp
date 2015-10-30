#include <QDoubleSpinBox>

#include "include/DoubleTableItemDelegate.h"

DoubleTableItemDelegate::DoubleTableItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QString DoubleTableItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    Q_UNUSED(locale)
    QString str = QString::number(value.toDouble(), 'f', 6);
    return str;
}

QWidget* DoubleTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(100);
    editor->setDecimals(6);

    return editor;
}
