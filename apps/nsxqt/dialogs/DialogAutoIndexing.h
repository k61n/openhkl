#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

namespace Ui
{
class DialogAutoIndexing;
}

class CollectedPeaksModel;
class ExperimentItem;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAutoIndexing(ExperimentItem* experiment_tree, const nsx::PeakList& peaks, QWidget* parent=nullptr);

    ~DialogAutoIndexing();

    void buildSolutionsTable();

private slots:

    void autoIndex();

    void removeUnitCells();

    void selectSolution(int);

    void slotResetUnitCell();

    virtual void reject() override;

    virtual void accept() override;

signals:

    void cellUpdated(nsx::sptrUnitCell);

private:

    Ui::DialogAutoIndexing *ui;

    ExperimentItem* _experiment_item;

    CollectedPeaksModel* _peaks_model;

    std::vector<std::pair<nsx::sptrPeak3D,std::shared_ptr<nsx::UnitCell>>> _defaults;

    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
