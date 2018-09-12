#pragma once

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;

namespace Ui {
class DialogPredictPeaks;
}

class DialogPredictPeaks : public QDialog
{
    Q_OBJECT

public:

    static DialogPredictPeaks* create(ExperimentItem* experiment_tree, const nsx::UnitCellList& peaks, QWidget* parent=nullptr);

    static DialogPredictPeaks* Instance();

    ~DialogPredictPeaks();

    double dMin() const;
    double dMax() const;
    double radius() const;
    double nFrames() const;

    int minNeighbors() const;

    int interpolation() const;

    nsx::sptrUnitCell cell(); 

public slots:

    virtual void accept() override;

private:

    DialogPredictPeaks(ExperimentItem* experiment_tree, const nsx::UnitCellList& unit_cells, QWidget* parent=nullptr);

    void predictPeaks();

    void slotActionClicked(QAbstractButton *button);

private:

    static DialogPredictPeaks *_instance;

    Ui::DialogPredictPeaks *_ui;

    ExperimentItem *_experiment_item;

    CollectedPeaksModel* _peaks_model;

};
