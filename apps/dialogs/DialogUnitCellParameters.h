#pragma once

#include <QDialog>

#include <core/CrystalTypes.h>

namespace Ui {
class DialogUnitCellParameters;
}

class Widget;

class DialogUnitCellParameters : public QDialog {
  Q_OBJECT

public:
  explicit DialogUnitCellParameters(nsx::sptrUnitCell unitCell,
                                    QWidget *parent = 0);
  ~DialogUnitCellParameters();

public slots:

  void setUnitCellParameters();

private:
  Ui::DialogUnitCellParameters *ui;
  nsx::sptrUnitCell _unitCell;
};
