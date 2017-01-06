#ifndef COLLECTEDPEAKSDELEGATE_H
#define COLLECTEDPEAKSDELEGATE_H

#include <QIcon>
#include <QStyledItemDelegate>

class CollectedPeaksDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CollectedPeaksDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:

    QIcon _icon;

};

#endif // COLLECTEDPEAKSDELEGATE_H
