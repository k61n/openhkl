
#ifndef AUTOINDEXER_H
#define AUTOINDEXER_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include <QTableView>

class AutoIndexer : public QcrFrame {
public:
    AutoIndexer();
private:
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
    QTableView* peaks;
    QTableView* solutions;
};

#endif //AUTOINDEXER_H
