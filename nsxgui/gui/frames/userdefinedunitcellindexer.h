//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/frames/userdefinedunitcellindexer.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_FRAMES_USERDEFINEDUNITCELLINDEXER_H
#define NSXGUI_GUI_FRAMES_USERDEFINEDUNITCELLINDEXER_H

#include "nsxgui/gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QTableView>
#include <core/CrystalTypes.h>
#include <core/UserDefinedUnitCellIndexer.h>

class UserDefinedUnitCellIndexer : public QcrFrame {
public:
    UserDefinedUnitCellIndexer();

private:
    void layout();
    void accept();
    void buildUnitCellsTable();
    void index();
    void resetPeaks();
    void slotActionClicked(QAbstractButton* button);
    void slotTabEdited(int index);
    void slotTabRemoved(int index);
    void slotSelectSolution(int);

    QcrTabWidget* tabwidget;
    QcrWidget* tab;
    PeaksTableView* peaktable;
    QcrDoubleSpinBox* a;
    QcrDoubleSpinBox* alpha;
    QcrDoubleSpinBox* wavelength;
    QcrDoubleSpinBox* gruber;
    QcrDoubleSpinBox* b;
    QcrDoubleSpinBox* beta;
    QcrDoubleSpinBox* niggli;
    QcrDoubleSpinBox* c;
    QcrDoubleSpinBox* gamma;
    QcrDoubleSpinBox* distance;
    QcrDoubleSpinBox* indexingTol;
    QcrDoubleSpinBox* angularTol;
    QcrDoubleSpinBox* indexingThreshold;
    QcrSpinBox* maxQVectors;
    QcrSpinBox* solutions;
    QcrCheckBox* onlyNiggli;
    QDialogButtonBox* buttons;
    QTableView* view;

    nsx::UserDefinedUnitCellIndexer indexer_;
    std::vector<std::pair<nsx::sptrPeak3D, nsx::sptrUnitCell>> defaults_;
    std::vector<std::pair<nsx::sptrUnitCell, double>> solutions_;
};

#endif // NSXGUI_GUI_FRAMES_USERDEFINEDUNITCELLINDEXER_H
