#pragma once

#include <QIcon>
#include <QStyledItemDelegate>

class CollectedPeaksDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CollectedPeaksDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:

    QIcon _icon;

};
