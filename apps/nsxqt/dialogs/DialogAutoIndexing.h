#pragma once

#include <memory>
#include <vector>

#include <QDialog>
#include <QItemSelection>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

namespace Ui
{
class DialogAutoIndexing;
}

class ExperimentItem;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAutoIndexing(ExperimentItem* experiment_item, const nsx::PeakList peaks, QWidget *parent=0);
    ~DialogAutoIndexing();

    void buildSolutionsTable();

public slots:
    void autoIndex();

private slots:

    void addUnitCell();

    void selectSolution(int);

signals:
    void cellUpdated(nsx::sptrUnitCell);

private:
    Ui::DialogAutoIndexing *ui;
    ExperimentItem* _experiment_item;
    nsx::PeakList _peaks;
    nsx::UnitCellList _unitCells;
    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
