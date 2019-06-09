//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogSpaceGroup.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <array>
#include <stdexcept>
#include <tuple>

#include <Eigen/Core>

#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTreeView>

#include "core/crystal/CrystalTypes.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/logger/Logger.h"
#include "core/crystal/MillerIndex.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/statistics/RFactor.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/instrument/Sample.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"

#include "apps/dialogs/DialogSpaceGroup.h"
#include "ui_DialogSpaceGroup.h"

DialogSpaceGroup::DialogSpaceGroup(const nsx::PeakList& peaks, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogSpaceGroup), _groups(), _cell(nullptr), _selectedGroup("")
{
    ui->setupUi(this);

    // Make sure that the user can not edit the content of the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cellin the table select the whole line.
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    nsx::PeakFilter peak_filter;
    _peaks = peak_filter.enabled(peaks, true);
    for (auto peak : _peaks) {
        auto current_peak_cell = peak->unitCell();
        if (!current_peak_cell) {
            continue;
        }

        if (!_cell) {
            _cell = current_peak_cell;
        } else {
            if (_cell != current_peak_cell) {
                throw std::runtime_error("Only one unit cell is supported at this time");
            }
        }
    }

    if (!_cell) {
        throw std::runtime_error("No unit cell defined for the selected peaks");
    }

    _peaks = peak_filter.indexed(_peaks, *_cell, _cell->indexingTolerance());

    if (_peaks.size() == 0) {
        throw std::runtime_error("Need at least one peak to find space group!");
    }

    evaluateSpaceGroups();

    buildTable();

    connect(ui->cancelOK, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->cancelOK, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogSpaceGroup::~DialogSpaceGroup()
{
    delete ui;
}

std::string DialogSpaceGroup::getSelectedGroup()
{
    return _selectedGroup;
}

void DialogSpaceGroup::evaluateSpaceGroups()
{
    auto&& symbols = nsx::SpaceGroup::symbols();

    nsx::MillerIndexList hkls;

    nsx::info() << "Retrieving reflection list for space group calculation...";

    for (auto peak : _peaks) {
        hkls.emplace_back(nsx::MillerIndex(peak->q(), *_cell).rowVector());
    }

    _groups.clear();

    nsx::info() << "Evaluating " << symbols.size() << " space groups based on " << hkls.size()
                << " peaks";

    auto compatible_space_groups = _cell->compatibleSpaceGroups();

    for (auto& symbol : compatible_space_groups) {

        nsx::SpaceGroup group = nsx::SpaceGroup(symbol);

        std::pair<std::string, double> entry =
            std::make_pair(symbol, 100.0 * (1 - group.fractionExtinct(hkls)));

        // group is compatible with observed reflections, so add it to list
        _groups.push_back(entry);
    }

    auto compare_fn = [](const std::pair<std::string, double>& a,
                         const std::pair<std::string, double>& b) -> bool {
        double quality_a = a.second;
        double quality_b = b.second;

        // sort first by quality
        if (quality_a != quality_b)
            return quality_a > quality_b;

        // otherwise we sort by properties of the groups
        nsx::SpaceGroup grp_a(a.first), grp_b(b.first);

        // sort by group ID
        return grp_a.id() < grp_b.id();
    };

    std::sort(_groups.begin(), _groups.end(), compare_fn);

    nsx::info() << "Done evaluating space groups.";
}

void DialogSpaceGroup::buildTable()
{
    QStandardItemModel* model = new QStandardItemModel(_groups.size(), 2, this);

    model->setHorizontalHeaderItem(0, new QStandardItem("Symbol"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Group ID"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(3, new QStandardItem("Agreement"));

    unsigned int row = 0;

    // Display solutions
    for (auto&& item : _groups) {

        const std::string& symbol = std::get<0>(item);
        double agreement = std::get<1>(item);
        nsx::SpaceGroup grp(symbol);

        QStandardItem* col0 = new QStandardItem(QString::fromStdString(symbol));
        QStandardItem* col1 = new QStandardItem(QString::number(grp.id()));
        QStandardItem* col2 = new QStandardItem(QString::fromStdString(grp.bravaisTypeSymbol()));
        QStandardItem* col3 = new QStandardItem(QString::number(agreement));

        model->setItem(row, 0, col0);
        model->setItem(row, 1, col1);
        model->setItem(row, 2, col2);
        model->setItem(row, 3, col3);

        ++row;
    }

    ui->tableView->setModel(model);
}

void DialogSpaceGroup::on_tableView_doubleClicked(const QModelIndex& index)
{
    _selectedGroup = std::get<0>(_groups[index.row()]);
    QMessageBox* box = new QMessageBox(this);
    box->setText(QString("Setting space group to ") + _selectedGroup.c_str());

    auto space_group = nsx::SpaceGroup(_selectedGroup);

    _cell->setSpaceGroup(space_group);

    box->exec();
}
