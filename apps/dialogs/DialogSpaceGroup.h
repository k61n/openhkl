//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogSpaceGroup.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>
#include <string>
#include <tuple>

#include <Eigen/Dense>

#include <QDialog>

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/crystal/SpaceGroup.h"

class QModelIndex;
class QWidget;

namespace Ui {
class DialogSpaceGroup;
}

class DialogSpaceGroup : public QDialog {
    Q_OBJECT

public:
    explicit DialogSpaceGroup(const nsx::PeakList& peaks, QWidget* parent = 0);
    ~DialogSpaceGroup();

    std::string getSelectedGroup();

private slots:
    void on_tableView_doubleClicked(const QModelIndex& index);

private:
    void evaluateSpaceGroups();
    void buildTable();

    Ui::DialogSpaceGroup* ui;
    nsx::PeakList _peaks;
    std::vector<std::pair<std::string, double>> _groups;
    nsx::sptrUnitCell _cell;
    std::string _selectedGroup;
};
