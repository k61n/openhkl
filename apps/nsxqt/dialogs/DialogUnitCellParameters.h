#ifndef NSXQT_DIALOGUNITCELLPARAMETERS_H
#define NSXQT_DIALOGUNITCELLPARAMETERS_H

#include <memory>

#include <QDialog>

namespace Ui
{
class DialogUnitCellParameters;
}

class Widget;

namespace nsx {
class UnitCell;
}

class DialogUnitCellParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUnitCellParameters(std::shared_ptr<nsx::UnitCell> unitCell, QWidget *parent=0);
    ~DialogUnitCellParameters();

public slots:

    void setUnitCellParameters();

private:
    Ui::DialogUnitCellParameters *ui;
    std::shared_ptr<nsx::UnitCell> _unitCell;
};

#endif // NSXQT_DIALOGUNITCELLPARAMETERS_H
