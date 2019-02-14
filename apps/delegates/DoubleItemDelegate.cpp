#include <QDoubleValidator>
#include <QLineEdit>

#include "DoubleItemDelegate.h"

QWidget *DoubleItemDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const {
  Q_UNUSED(option)
  Q_UNUSED(index)

  QLineEdit *lineEdit = new QLineEdit(parent);

  // Set validator
  QDoubleValidator *validator = new QDoubleValidator(lineEdit);
  lineEdit->setValidator(validator);

  return lineEdit;
}
