#ifndef NSXQT_DIALOGAUTOINDEXING_H
#define NSXQT_DIALOGAUTOINDEXING_H

#include <memory>
#include <vector>

#include <QDialog>
#include <QItemSelection>

#include <nsxlib/crystal/CrystalTypes.h>

namespace Ui
{
class DialogAutoIndexing;
}

namespace nsx
{
class Experiment;
}

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;

    explicit DialogAutoIndexing(sptrExperiment experiment, const nsx::PeakList peaks, QWidget *parent=0);
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

    sptrExperiment _experiment;

    nsx::PeakList _peaks;

    nsx::UnitCellList _unitCells;

    std::vector<std::pair<nsx::UnitCell,double>> _solutions;
};

#endif // NSXQT_DIALOGAUTOINDEXING_H
