#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QPixmap>
#include <QtDebug>

#include "CollectedPeaksModel.h"
#include "CollectedPeaksDelegate.h"

CollectedPeaksDelegate::CollectedPeaksDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _icon.addPixmap(QPixmap(":/resources/peakSelectedIcon.png"),QIcon::Normal,QIcon::On);
    _icon.addPixmap(QPixmap(":/resources/peakDeselectedIcon.png"),QIcon::Disabled,QIcon::On);
}


QWidget* CollectedPeaksDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == CollectedPeaksModel::Column::unitCell)
    {
        QComboBox *editor = new QComboBox(parent);

        QStringList cellNames = index.model()->data(index,Qt::UserRole).toStringList();
        for (const auto& name : cellNames)
            editor->addItem(name);
        return editor;
    }
    else
        return QStyledItemDelegate::createEditor(parent,option,index);
}

void CollectedPeaksDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    int column = index.column();

    if (column = CollectedPeaksModel::Column::unitCell)
    {
        QComboBox *cb = static_cast<QComboBox*>(editor);
        int unitCellIndex = cb->currentIndex();
        model->setData(index,unitCellIndex);
    }
    else
        QStyledItemDelegate::setModelData(editor,model,index);
}

void CollectedPeaksDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int column = index.column();
    if (column == CollectedPeaksModel::Column::selected || column == CollectedPeaksModel::Column::observed)
    {
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
    else
        QStyledItemDelegate::paint(painter,option,index);

}

bool CollectedPeaksDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(option);
    if(event->type() == QEvent::MouseButtonRelease)
    {
        int column = index.column();
        if (column == CollectedPeaksModel::Column::selected || column == CollectedPeaksModel::Column::observed)
        {
            bool value  = model->data(index,Qt::CheckStateRole).toBool();
            model->setData(index, !value, Qt::CheckStateRole);
        }
    }
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}
