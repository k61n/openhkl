#pragma once

#include <map>

#include <QPersistentModelIndex>
#include <QStyleOptionButton>
#include <QStyledItemDelegate>

class QMouseEvent;
class QWidget;

class TaskManagerViewDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    TaskManagerViewDelegate(QWidget* parent);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)
        const override;

    bool editorEvent(
        QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
        const QModelIndex& index) override;

signals:

    void actionButtonPressed(QModelIndex);

private:
    mutable std::map<QModelIndex, QStyleOptionButton> _button_states;
};
