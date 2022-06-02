//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/UnitCellWidget.h
//! @brief     Defines class UnitCellWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_FRAMES_UNITCELLWIDGET_H
#define NSX_GUI_FRAMES_UNITCELLWIDGET_H

#include "tables/crystal/UnitCell.h"
#include <QTableView>

//! Tab of an indexer frame which shows the parameters of the selected unit cell
class UnitCellWidget : public QWidget {
    Q_OBJECT
 public:
    UnitCellWidget(nsx::sptrUnitCell, const QString&);
    nsx::sptrUnitCell unitCell() const { return unitCell_; }
    bool spaceGroupSet() { return wasSpaceGroupSet; }
    void setSpaceGroup();

 private:
    void evaluateSpaceGroups();

    nsx::sptrUnitCell unitCell_;
    QTableView* spaceGroupView;
    bool wasSpaceGroupSet;
    std::string spaceGroupOne;
};

#endif // NSX_GUI_FRAMES_UNITCELLWIDGET_H
