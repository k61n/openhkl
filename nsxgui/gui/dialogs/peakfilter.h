
#ifndef NSXGUI_GUI_DIALOGS_PEAKFILTER_H
#define NSXGUI_GUI_DIALOGS_PEAKFILTER_H

#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

#include <QDialogButtonBox>
#include <QGroupBox>

class PeakFilter : public QDialog {
public:
    PeakFilter();
    ~PeakFilter();

public slots:
    void accept() override;
    void slotActionClicked(QAbstractButton* button);
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


#endif // NSXGUI_GUI_DIALOGS_PEAKFILTER_H
