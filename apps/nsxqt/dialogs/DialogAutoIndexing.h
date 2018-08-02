#pragma once

#include <memory>
#include <vector>

#include <QDialog>

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

    explicit DialogAutoIndexing(const nsx::PeakList peaks, QWidget *parent=0);
    ~DialogAutoIndexing();
    void buildSolutionsTable();

    nsx::sptrUnitCell unitCell();

public slots:
    void autoIndex();

private slots:
    //void addUnitCell();

    void selectSolution(int);

signals:
    void cellUpdated(nsx::sptrUnitCell);

private:
    Ui::DialogAutoIndexing *ui;
    ExperimentItem* _experiment_item;
    nsx::PeakList _peaks;
    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;

    nsx::sptrUnitCell _unitCell;
};
