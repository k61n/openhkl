#pragma once

#include <memory>
#include <vector>

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

namespace Ui
{
class DialogAutoIndexing;
}

class ExperimentItem;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAutoIndexing(ExperimentItem* experiment_tree, const nsx::PeakList peaks, QWidget* parent=nullptr);

    ~DialogAutoIndexing();

    void buildSolutionsTable();

private slots:

    void autoIndex();

    void removeUnitCells();

    void selectSolution(int);

    virtual void accept() override;

signals:

    void cellUpdated(nsx::sptrUnitCell);

private:

    Ui::DialogAutoIndexing *ui;

    ExperimentItem* _experiment_item;

    nsx::PeakList _peaks;

    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
