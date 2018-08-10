#pragma once

#include <QIcon>
#include <QStyledItemDelegate>

class CollectedPeaksDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CollectedPeaksDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:

    QIcon _icon;

};
