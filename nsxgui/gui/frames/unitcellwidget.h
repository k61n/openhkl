
#ifndef UNITCELLWIDGET_H
#define UNITCELLWIDGET_H

#include <core/CrystalTypes.h>
#include <QCR/widgets/views.h>
#include <QCR/widgets/controls.h>

class UnitCellWidget : public QcrWidget {
  Q_OBJECT
public:
  UnitCellWidget(nsx::sptrUnitCell, const QString&);
  nsx::sptrUnitCell unitCell() const { return unitCell_; }
private:
  nsx::sptrUnitCell unitCell_;
};

#endif //UNITCELLWIDGET_H

