#ifndef NSXQT_DIALOGAUTOINDEXING_H
#define NSXQT_DIALOGAUTOINDEXING_H

#include <memory>
#include <vector>

#include <QDialog>
#include <QItemSelection>

#include <nsxlib/crystal/UnitCell.h>

namespace Ui
{
class DialogAutoIndexing;
}

namespace nsx
{
class Experiment;
class Peak3D;
}

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;
    using sptrPeak3D = std::shared_ptr<nsx::Peak3D>;
    using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;
    using CellList = std::vector<sptrUnitCell>;

    explicit DialogAutoIndexing(sptrExperiment experiment, const std::vector<sptrPeak3D> peaks, QWidget *parent=0);
    ~DialogAutoIndexing();

    void buildSolutionsTable();

public slots:

    void autoIndex();

private slots:

    void selectSolution(int);

signals:

    void cellUpdated(sptrUnitCell);

private:
    Ui::DialogAutoIndexing *ui;

    sptrExperiment _experiment;

    std::vector<sptrPeak3D> _peaks;

    CellList _unitCells;

    std::vector<std::pair<nsx::UnitCell,double>> _solutions;
};

#endif // NSXQT_DIALOGAUTOINDEXING_H
