//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/UnitCellWidget.h
//! @brief     Defines class UnitCellWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_FRAMES_UNITCELLWIDGET_H
#define OHKL_GUI_FRAMES_UNITCELLWIDGET_H

#include "core/data/DataTypes.h"

#include <QTableView>

//! Tab of an indexer frame which shows the parameters of the selected unit cell
class UnitCellWidget : public QWidget {
    Q_OBJECT
 public:
    UnitCellWidget(ohkl::sptrUnitCell, const QString&);
    ohkl::sptrUnitCell unitCell() const { return unitCell_; }
    bool spaceGroupSet() { return wasSpaceGroupSet; }
    void setSpaceGroup();

 private:
    void evaluateSpaceGroups();

    ohkl::sptrUnitCell unitCell_;
    QTableView* spaceGroupView;
    bool wasSpaceGroupSet;
    std::string spaceGroupOne;
};

#endif // OHKL_GUI_FRAMES_UNITCELLWIDGET_H
