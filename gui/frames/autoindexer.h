//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/autoindexer.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_FRAMES_AUTOINDEXER_H
#define NSXGUI_GUI_FRAMES_AUTOINDEXER_H

#include "gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QTableView>
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

    void slotActionClicked(QAbstractButton* button);
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

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<nsx::UnitCell>>> _defaults;
    std::vector<std::pair<nsx::sptrUnitCell, double>> _solutions;
};

#endif // NSXGUI_GUI_FRAMES_AUTOINDEXER_H
