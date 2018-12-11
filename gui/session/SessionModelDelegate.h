#pragma once

#include <QStyledItemDelegate>

class SessionModelDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    SessionModelDelegate();

    virtual void paint(
        QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    virtual bool editorEvent(
        QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
        const QModelIndex& index);
};
