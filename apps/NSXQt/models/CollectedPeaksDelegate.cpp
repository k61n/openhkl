#include <QApplication>
#include <QCheckBox>
#include <QPainter>
#include <QPixmap>

#include "CollectedPeaksModel.h"
#include "CollectedPeaksDelegate.h"

CollectedPeaksDelegate::CollectedPeaksDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _icon.addPixmap(QPixmap(":/resources/peakSelectedIcon.png"),QIcon::Normal,QIcon::On);
    _icon.addPixmap(QPixmap(":/resources/peakDeselectedIcon.png"),QIcon::Disabled,QIcon::On);
}


void CollectedPeaksDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);
    int col = index.column();
    if (col == CollectedPeaksModel::Column::selected || col == CollectedPeaksModel::Column::observed)
    {
        bool value  = index.model()->data(index,Qt::UserRole).toBool();
        QStyleOptionButton buttonVis;
        buttonVis.rect = option.rect;
        int h = option.rect.height()/2 + 1;
        buttonVis.iconSize = QSize(h,h);
        buttonVis.icon = _icon;
        buttonVis.features |= QStyleOptionButton::Flat;
        buttonVis.state |= value ? QStyle::State_Enabled : QStyle::State_None;
        QApplication::style()->drawControl(QStyle::CE_PushButton,&buttonVis,painter);
    }
}

bool CollectedPeaksDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        bool value  = model->data(index,Qt::UserRole).toBool();
        model->setData(index, !value, Qt::UserRole);
    }
    return true;
}
