//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/RefinerTables.cpp
//! @brief     Implements tables for SubframeRefiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/Units.h" // deg

#include "gui/subframe_refiner/RefinerTables.h"

#include <QVBoxLayout>
#include <QDebug>

RefinerTables::RefinerTables()
{
    setSizePolicies();
    _lattice_tab = new QWidget();
    _sample_position_tab = new QWidget();
    _sample_orientation_tab = new QWidget();
    _detector_orientation_tab = new QWidget();
    _ki_tab = new QWidget();

    addTab(_lattice_tab, "Unit cell");
    addTab(_sample_position_tab, "Sample position");
    addTab(_sample_orientation_tab, "Sample orientation");
    addTab(_detector_orientation_tab, "Detector_orientation");
    addTab(_ki_tab, "Incident wavevector");

    setLatticeTableUp();
}

void RefinerTables::refreshTables(nsx::Refiner* refiner)
{
    refreshLatticeTable(refiner);
}

void RefinerTables::setSizePolicies()
{
}

void RefinerTables::setLatticeTableUp()
{
    QVBoxLayout* lattice_layout = new QVBoxLayout(_lattice_tab);

    _lattice_view = new QTableView;
    _lattice_model = new QStandardItemModel(0, 8, this);
    _lattice_view->setModel(_lattice_model);
    _lattice_model->setHorizontalHeaderLabels(
        {"fmin", "fmax", "a", "b", "c", QString((QChar)0x03B1), QString((QChar)0x03B2),
        QString((QChar)0x03B3)});
    lattice_layout->addWidget(_lattice_view);
}

void RefinerTables::refreshLatticeTable(nsx::Refiner* refiner)
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_lattice_view->model());
    model->removeRows(0, model->rowCount());

    if (refiner == nullptr)
        return;

    auto batches = refiner->batches();

    if (batches.size() == 0)
        return;

    for (auto batch : batches) {
        QList<QStandardItem*> row;
        auto c = batch.cell()->character();
        row.push_back(new QStandardItem(QString::number(batch.fmin())));
        row.push_back(new QStandardItem(QString::number(batch.fmax())));
        row.push_back(new QStandardItem(QString::number(c.a)));
        row.push_back(new QStandardItem(QString::number(c.b)));
        row.push_back(new QStandardItem(QString::number(c.c)));
        row.push_back(new QStandardItem(QString::number(c.alpha / nsx::deg)));
        row.push_back(new QStandardItem(QString::number(c.beta / nsx::deg)));
        row.push_back(new QStandardItem(QString::number(c.gamma / nsx::deg)));
        model->appendRow(row);
    }
}
