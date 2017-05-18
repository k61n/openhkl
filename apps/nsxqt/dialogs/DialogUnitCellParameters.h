#ifndef DIALOGUNITCELLPARAMETERS_H
#define DIALOGUNITCELLPARAMETERS_H

#include <QDialog>

#include <nsxlib/utils/Types.h>

namespace Ui
{
class DialogUnitCellParameters;
}

class Widget;

class DialogUnitCellParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUnitCellParameters(nsx::sptrUnitCell unitCell, QWidget *parent=0);
    ~DialogUnitCellParameters();

public slots:

    void setUnitCellParameters();

private:
    Ui::DialogUnitCellParameters *ui;
    nsx::sptrUnitCell _unitCell;
};

#endif // DIALOGUNITCELLPARAMETERS_H
