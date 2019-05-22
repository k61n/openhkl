
#ifndef PEAKFILTER_H
#define PEAKFILTER_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include "nsxgui/gui/models/peakstable.h"
#include "nsxgui/gui/models/experimentmodel.h"

#include <QGroupBox>
#include <QDialogButtonBox>

class PeakFilter : public QDialog {
public:
    PeakFilter();
    ~PeakFilter();

public slots:
    void accept() override;
    void slotActionClicked(QAbstractButton *button);
    void slotUnitCellChanged(int index);

private:
    void doLayout();
    void filterPeaks();

    QcrCheckBox* selected;
    QcrCheckBox* masked;
    QcrCheckBox* predicted;
    QcrCheckBox* indexedPeak;
    QcrCheckBox* sapcegroupExtincted;
    QcrCheckBox* removeOverlapping;
    QcrCheckBox* keepComplementary;
    QcrComboBox* unitCell;
    QcrDoubleSpinBox* tolerance;
    QcrDoubleSpinBox* strengthMin;
    QcrDoubleSpinBox* strengthMax;
    QcrDoubleSpinBox* drangeMin;
    QcrDoubleSpinBox* drangeMax;
    QcrDoubleSpinBox* significanceLevel;
    QcrSpinBox* minNumberPeaks;
    PeaksTableView* peaksTable;
    QGroupBox* stateBox;
    QGroupBox* byUnitCell;
    QGroupBox* strengthBox;
    QGroupBox* rangeBox;
    QGroupBox* sparseGroup;
    QGroupBox* mergeGroup;
    QDialogButtonBox* buttons;

    nsx::PeakList peaks_;
    PeaksTableModel* model_;
};


#endif //PEAKFILTER_H
