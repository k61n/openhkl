#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QKeyEvent>
#include <QModelIndex>
#include <QPainter>
#include <QPixmap>

#include "CollectedPeaksDelegate.h"
#include "CollectedPeaksModel.h"

#include <QDebug>

CollectedPeaksDelegate::CollectedPeaksDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _icon.addPixmap(QPixmap(":/resources/peakSelectedIcon.png"),QIcon::Normal,QIcon::On);
    _icon.addPixmap(QPixmap(":/resources/peakDeselectedIcon.png"),QIcon::Disabled,QIcon::On);
}

void CollectedPeaksDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int column = index.column();
    if (column == CollectedPeaksModel::Column::selected) {
        bool value  = index.model()->data(index,Qt::CheckStateRole).toBool();
        QStyleOptionButton buttonVis;
        buttonVis.rect = option.rect;
        int h = option.rect.height()/2 + 1;
        buttonVis.iconSize = QSize(h,h);
        buttonVis.icon = _icon;
        buttonVis.features |= QStyleOptionButton::Flat;
        buttonVis.state |= value ? QStyle::State_Enabled : QStyle::State_None;
        QApplication::style()->drawControl(QStyle::CE_PushButton,&buttonVis,painter);
    }
    else {
        QStyledItemDelegate::paint(painter,option,index);
    }
}
