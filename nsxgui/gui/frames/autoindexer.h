
#ifndef AUTOINDEXER_H
#define AUTOINDEXER_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include "nsxgui/gui/models/peakstable.h"
#include <QTableView>
#include <QDialogButtonBox>
#include <build/core/include/core/Peak3D.h>

class AutoIndexer : public QcrFrame {
public:
    AutoIndexer();
private:
    void layout();
    void accept();
    void buildSolutionsTable();
    void resetUnitCell();
    void run();

    void slotActionClicked(QAbstractButton *button);
    void slotTabEdited(int index);
    void slotTabRemoved(int index);
    void selectSolution(int);

    QcrTabWidget* tabs;
    QcrWidget* settings;
    QcrDoubleSpinBox* gruber;
    QcrDoubleSpinBox* niggli;
    QcrDoubleSpinBox* maxCellDim;
    QcrDoubleSpinBox* minCellVolume;
    QcrDoubleSpinBox* indexingTolerance;
    QcrSpinBox* nVertices;
    QcrSpinBox* nSolutions;
    QcrSpinBox* nSubdivisions;
    QcrCheckBox* only_niggli;
    PeaksTableView* peaks;
    PeaksTableModel* model;
    QTableView* solutions;
    QDialogButtonBox* buttons;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<nsx::UnitCell>>>
        _defaults;
    std::vector<std::pair<nsx::sptrUnitCell, double>> _solutions;
};

#endif //AUTOINDEXER_H
