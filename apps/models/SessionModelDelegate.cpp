#include <QApplication>
#include <QStyleOptionButton>

#include "SessionModel.h"
#include "SessionModelDelegate.h"
#include "UnitCellsItem.h"

SessionModelDelegate::SessionModelDelegate() {}

void SessionModelDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
  // Case of an invalid index, call the parent paint method
  if (!index.isValid()) {
    return QStyledItemDelegate::paint(painter, option, index);
  }

  const SessionModel *session_model =
      dynamic_cast<const SessionModel *>(index.model());
  // if the underlying model is not a session model (should not happen) call the
  // parent paint method
  if (!session_model) {
    return QStyledItemDelegate::paint(painter, option, index);
  }

  auto unit_cells_item = dynamic_cast<UnitCellsItem *>(
      session_model->itemFromIndex(index.parent()));
  if (!unit_cells_item) {
    return QStyledItemDelegate::paint(painter, option, index);
  }

  auto widget = option.widget;

  auto style = widget ? widget->style() : QApplication::style();

  QStyleOptionButton *opt = new QStyleOptionButton();

  opt->rect = option.rect;
  opt->text = index.data(Qt::DisplayRole).toString();
  opt->state |= QStyle::State_Enabled |
                (index.data(Qt::CheckStateRole).toBool() ? QStyle::State_On
                                                         : QStyle::State_Off);

  style->drawControl(QStyle::CE_RadioButton, opt, painter);
}

bool SessionModelDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) {
  const SessionModel *session_model = dynamic_cast<const SessionModel *>(model);
  // if the underlying model is not a session model (should not happen) call the
  // parent paint method
  if (!session_model) {
    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }

  auto unit_cells_item = dynamic_cast<UnitCellsItem *>(
      session_model->itemFromIndex(index.parent()));
  if (!unit_cells_item) {
    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }

  auto value = QStyledItemDelegate::editorEvent(event, model, option, index);

  auto selected_unit_cell = unit_cells_item->child(index.row());

  if (value) {
    if (event->type() == QEvent::MouseButtonRelease) {
      if (index.data(Qt::CheckStateRole) == Qt::Checked) {
        for (auto i = 0; i < unit_cells_item->rowCount(); ++i) {
          if (i == index.row()) {
            continue;
          }
          auto other_unit_cell_item = unit_cells_item->child(i);
          other_unit_cell_item->setData(Qt::Unchecked, Qt::CheckStateRole);
        }
      }
    }
  }

  return value;
}
