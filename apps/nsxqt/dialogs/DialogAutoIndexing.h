#ifndef NSXQT_DIALOGAUTOINDEXING_H
#define NSXQT_DIALOGAUTOINDEXING_H

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

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAutoIndexing(nsx::sptrExperiment experiment, const nsx::PeakList peaks, QWidget *parent=0);
    ~DialogAutoIndexing();

    void buildSolutionsTable();

public slots:
    void autoIndex();

private slots:
    void selectSolution(int);

signals:
    void cellUpdated(nsx::sptrUnitCell);

private:
    Ui::DialogAutoIndexing *ui;
    nsx::sptrExperiment _experiment;
    nsx::PeakList _peaks;
    nsx::UnitCellList _unitCells;
    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};

#endif // NSXQT_DIALOGAUTOINDEXING_H
