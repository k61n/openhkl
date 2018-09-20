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
class QAbstractButton;
class QListWidgetItem;

class DialogAutoIndexing : public QDialog
{
    Q_OBJECT

public:

    static DialogAutoIndexing* create(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent=nullptr);

    static DialogAutoIndexing* Instance();

    ~DialogAutoIndexing();

    void buildSolutionsTable();

public slots:

    virtual void accept() override;

    virtual void reject() override;

private slots:

    void slotActionClicked(QAbstractButton* button);

    void autoIndex();

    void removeUnitCells();

    void selectSolution(int);

    void slotEditUnitCellName(QListWidgetItem *item);

signals:

    void cellUpdated(nsx::sptrUnitCell);

private:

    //! Private ctor for Singleton pattern
    DialogAutoIndexing(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget *parent=nullptr);

    void resetUnitCell();

private:

    static DialogAutoIndexing *_instance;

    Ui::DialogAutoIndexing *_ui;

    ExperimentItem* _experiment_item;

    CollectedPeaksModel* _peaks_model;

    std::vector<std::pair<nsx::sptrPeak3D,std::shared_ptr<nsx::UnitCell>>> _defaults;

    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
