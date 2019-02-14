#pragma once

#include <QDialog>

class QWidget;

namespace Ui {
class DialogIsotopesDatabase;
}

class DialogIsotopesDatabase : public QDialog {
  Q_OBJECT

public:
  explicit DialogIsotopesDatabase(QWidget *parent = 0);

  ~DialogIsotopesDatabase();

private:
  Ui::DialogIsotopesDatabase *ui;
};
