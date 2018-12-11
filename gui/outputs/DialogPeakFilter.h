#pragma once

#include <string>

#include <QDialog>

#include <core/PeakList.h>

class QAbstractButton;

namespace Ui {
class DialogPeakFilter;
}

class CollectedPeaksModel;
class ExperimentItem;

class DialogPeakFilter : public QDialog {
    Q_OBJECT

public:
    static DialogPeakFilter* create(ExperimentItem* experiment_tree, const nsx::PeakList& peaks, QWidget* parent=nullptr);

    static DialogPeakFilter* Instance();

    virtual ~DialogPeakFilter();

public slots:
    virtual void accept() override;

    void slotActionClicked(QAbstractButton *button);

    void slotUnitCellChanged(int index);

private:
    DialogPeakFilter(ExperimentItem* experiment_tree, const nsx::PeakList& peaks, QWidget* parent=nullptr);

    void filterPeaks();

private:
    static DialogPeakFilter *_instance;
    Ui::DialogPeakFilter *_ui;
    ExperimentItem *_experiment_item;
    CollectedPeaksModel *_peaks_model;
    nsx::PeakList _peaks;
};
