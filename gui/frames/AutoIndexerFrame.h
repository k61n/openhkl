//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/AutoIndexerFrame.h
//! @brief     Defines class AutoIndexerFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_AUTOINDEXERFRAME_H
#define GUI_FRAMES_AUTOINDEXERFRAME_H

#include "core/peak/Peak3D.h"
#include "gui/views/PeakTableView.h"
#include "gui/models/PeakModel.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>

//! Frame which shows the settings for the AutoIndexer
class AutoIndexerFrame : public QcrFrame {
 public:
    AutoIndexerFrame();

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
    QcrComboBox* listNames;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<nsx::UnitCell>>> _defaults;
    std::vector<std::pair<nsx::sptrUnitCell, double>> _solutions;
};

#endif // GUI_FRAMES_AUTOINDEXERFRAME_H
