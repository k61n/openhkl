#pragma once

#include <QStyledItemDelegate>

class DoubleTableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DoubleTableItemDelegate(QObject *parent = 0);

    QString displayText(const QVariant& value, const QLocale& locale) const;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
