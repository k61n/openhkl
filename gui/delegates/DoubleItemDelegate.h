#pragma once

#include <QItemDelegate>
#include <QModelIndex>
#include <QStyleOptionViewItem>

class DoubleItemDelegate : public QItemDelegate
{
public:
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
};

