#ifndef NSXQT_DIALOGAUTOINDEXING_H
#define NSXQT_DIALOGAUTOINDEXING_H

#include <memory>
#include <vector>

#include <QDialog>
#include <QItemSelection>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/Types.h>

namespace Ui
{
    class DialogAutoIndexing;
}

namespace nsx
{
    class Experiment;
    class Peak3D;
}

using namespace nsx;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAutoIndexing(std::shared_ptr<Experiment> experiment, const std::vector<sptrPeak3D> peaks, QWidget *parent=0);
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

    std::shared_ptr<Experiment> _experiment;

    std::vector<sptrPeak3D> _peaks;

    nsx::CellList _unitCells;

    std::vector<std::pair<sptrUnitCell,double>> _solutions;
};

#endif // NSXQT_DIALOGAUTOINDEXING_H
