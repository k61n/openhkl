#pragma once

#include <map>
#include <string>

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

class QAbstractButton;

namespace Ui {
class DialogPeakFilter;
}

class CollectedPeaksModel;
class ExperimentItem;

class DialogPeakFilter : public QDialog {
    Q_OBJECT

public:
    DialogPeakFilter(ExperimentItem* experiment_tree, const nsx::PeakList& peaks, QWidget* parent=nullptr);

    virtual ~DialogPeakFilter();

    const nsx::PeakList& filteredPeaks() const;

public slots:

    virtual void accept() override;

    void slotActionClicked(QAbstractButton *button);

    void slotUnitCellChanged(int index);

private:

    void filterPeaks();

private:
    Ui::DialogPeakFilter* _ui;

    ExperimentItem* _experiment_item;

    CollectedPeaksModel* _peaks_model;

    nsx::PeakList _peaks;

    nsx::PeakList _filtered_peaks;
};
