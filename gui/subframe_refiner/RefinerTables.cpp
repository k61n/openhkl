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
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InstrumentStateList.h"

#include "gui/subframe_refiner/RefinerTables.h"

#include <QDebug>
#include <QVBoxLayout>

RefinerTables::RefinerTables()
{
    _lattice_tab = new QWidget();
    _sample_pos_tab = new QWidget();
    _sample_orn_tab = new QWidget();
    _detector_pos_tab = new QWidget();
    _ki_tab = new QWidget();

    addTab(_lattice_tab, "Unit cell");
    addTab(_sample_pos_tab, "Sample position");
    addTab(_sample_orn_tab, "Sample orientation");
    addTab(_detector_pos_tab, "Detector position");
    addTab(_ki_tab, "Incident wavevector");

    setLatticeTableUp();
    setSamplePosTableUp();
    setSampleOrnTableUp();
    setDetectorPosTableUp();
    setKiTableUp();
}

void RefinerTables::refreshTables(nsx::Refiner* refiner, nsx::DataSet* data)
{
    refreshLatticeTable(refiner);
    refreshSamplePosTable(refiner, data);
    refreshSampleOrnTable(refiner, data);
    refreshDetectorPosTable(refiner, data);
    refreshKiTable(refiner, data);
}

void RefinerTables::setLatticeTableUp()
{
    QVBoxLayout* lattice_layout = new QVBoxLayout(_lattice_tab);

    _original_lattice_view = new QTableView;
    _original_lattice_model = new QStandardItemModel(0, 8, this);
    _original_lattice_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    _original_lattice_view->setModel(_original_lattice_model);
    _original_lattice_model->setHorizontalHeaderLabels(
        {"fmin", "fmax", "a", "b", "c", QString((QChar)0x03B1), QString((QChar)0x03B2),
         QString((QChar)0x03B3)});
    _lattice_view = new QTableView;
    _lattice_model = new QStandardItemModel(0, 8, this);
    _lattice_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _lattice_view->setModel(_lattice_model);
    _lattice_model->setHorizontalHeaderLabels(
        {"fmin", "fmax", "a", "b", "c", QString((QChar)0x03B1), QString((QChar)0x03B2),
         QString((QChar)0x03B3)});
    lattice_layout->addWidget(_original_lattice_view);
    lattice_layout->addWidget(_lattice_view);
}

void RefinerTables::refreshLatticeTable(nsx::Refiner* refiner)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_lattice_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_lattice_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    QList<QStandardItem*> row;
    auto c = refiner->unrefinedCell()->character();
    row.push_back(new QStandardItem(QString::number(1)));
    row.push_back(new QStandardItem(QString::number(refiner->nframes())));
    row.push_back(new QStandardItem(QString::number(c.a)));
    row.push_back(new QStandardItem(QString::number(c.b)));
    row.push_back(new QStandardItem(QString::number(c.c)));
    row.push_back(new QStandardItem(QString::number(c.alpha / nsx::deg)));
    row.push_back(new QStandardItem(QString::number(c.beta / nsx::deg)));
    row.push_back(new QStandardItem(QString::number(c.gamma / nsx::deg)));
    m0->appendRow(row);

    if (refiner == nullptr)
        return;

    auto batches = refiner->batches();

    if (batches.size() == 0)
        return;

    for (auto batch : batches) {
        QList<QStandardItem*> row;
        auto c = batch.cell()->character();
        row.push_back(new QStandardItem(QString::number(batch.first_frame())));
        row.push_back(new QStandardItem(QString::number(batch.last_frame() - 2)));
        row.push_back(new QStandardItem(QString::number(c.a)));
        row.push_back(new QStandardItem(QString::number(c.b)));
        row.push_back(new QStandardItem(QString::number(c.c)));
        row.push_back(new QStandardItem(QString::number(c.alpha / nsx::deg)));
        row.push_back(new QStandardItem(QString::number(c.beta / nsx::deg)));
        row.push_back(new QStandardItem(QString::number(c.gamma / nsx::deg)));
        model->appendRow(row);
    }
}

void RefinerTables::setSamplePosTableUp()
{
    QHBoxLayout* sample_pos_layout = new QHBoxLayout(_sample_pos_tab);

    _original_sample_pos_view = new QTableView;
    _original_sample_pos_model = new QStandardItemModel(0, 1, this);
    _original_sample_pos_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _original_sample_pos_view->setModel(_original_sample_pos_model);
    _original_sample_pos_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    _sample_pos_view = new QTableView;
    _sample_pos_model = new QStandardItemModel(0, 1, this);
    _sample_pos_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _sample_pos_view->setModel(_sample_pos_model);
    _sample_pos_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    sample_pos_layout->addWidget(_original_sample_pos_view);
    sample_pos_layout->addWidget(_sample_pos_view);
}

void RefinerTables::refreshSamplePosTable(nsx::Refiner* refiner, nsx::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_sample_pos_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_sample_pos_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const nsx::InstrumentState& state : *refiner->unrefinedStates()) {
        QList<QStandardItem*> row;
        auto pos = state.samplePosition;
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(pos[0])));
        row.push_back(new QStandardItem(QString::number(pos[1])));
        row.push_back(new QStandardItem(QString::number(pos[2])));
        m0->appendRow(row);
        ++frame;
    }

    if (refiner == nullptr)
        return;

    if (data == nullptr)
        return;

    frame = 1;
    for (const nsx::InstrumentState& state : *refiner->refinedStates()) {
        QList<QStandardItem*> row;
        auto pos = state.samplePosition;
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(pos[0])));
        row.push_back(new QStandardItem(QString::number(pos[1])));
        row.push_back(new QStandardItem(QString::number(pos[2])));
        model->appendRow(row);
        ++frame;
    }
}

void RefinerTables::setSampleOrnTableUp()
{
    QHBoxLayout* sample_orn_layout = new QHBoxLayout(_sample_orn_tab);

    _original_sample_orn_view = new QTableView;
    _original_sample_orn_model = new QStandardItemModel(0, 10, this);
    _original_sample_orn_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    _original_sample_orn_view->setModel(_original_sample_orn_model);
    _original_sample_orn_model->setHorizontalHeaderLabels({
        "frame", "xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"});
    _sample_orn_view = new QTableView;
    _sample_orn_model = new QStandardItemModel(0, 10, this);
    _sample_orn_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _sample_orn_view->setModel(_sample_orn_model);
    _sample_orn_model->setHorizontalHeaderLabels({
      "frame", "xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"});
    sample_orn_layout->addWidget(_original_sample_orn_view);
    sample_orn_layout->addWidget(_sample_orn_view);
}

void RefinerTables::refreshSampleOrnTable(nsx::Refiner* refiner, nsx::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_sample_orn_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_sample_orn_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const nsx::InstrumentState& state : *refiner->unrefinedStates()) {
        QList<QStandardItem*> row;
        auto mat = state.sampleOrientationMatrix();
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(mat(0, 0))));
        row.push_back(new QStandardItem(QString::number(mat(0, 1))));
        row.push_back(new QStandardItem(QString::number(mat(0, 2))));
        row.push_back(new QStandardItem(QString::number(mat(1, 0))));
        row.push_back(new QStandardItem(QString::number(mat(1, 1))));
        row.push_back(new QStandardItem(QString::number(mat(1, 2))));
        row.push_back(new QStandardItem(QString::number(mat(2, 0))));
        row.push_back(new QStandardItem(QString::number(mat(2, 1))));
        row.push_back(new QStandardItem(QString::number(mat(2, 2))));
        m0->appendRow(row);
        ++frame;
    }

    if (refiner == nullptr)
        return;

    if (data == nullptr)
        return;

    frame = 1;
    for (const nsx::InstrumentState& state : *refiner->refinedStates()) {
        QList<QStandardItem*> row;
        auto mat = state.sampleOrientationMatrix();
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(mat(0, 0))));
        row.push_back(new QStandardItem(QString::number(mat(0, 1))));
        row.push_back(new QStandardItem(QString::number(mat(0, 2))));
        row.push_back(new QStandardItem(QString::number(mat(1, 0))));
        row.push_back(new QStandardItem(QString::number(mat(1, 1))));
        row.push_back(new QStandardItem(QString::number(mat(1, 2))));
        row.push_back(new QStandardItem(QString::number(mat(2, 0))));
        row.push_back(new QStandardItem(QString::number(mat(2, 1))));
        row.push_back(new QStandardItem(QString::number(mat(2, 2))));
        model->appendRow(row);
        ++frame;
    }
}

void RefinerTables::setDetectorPosTableUp()
{
    QHBoxLayout* detector_pos_layout = new QHBoxLayout(_detector_pos_tab);

    _original_detector_pos_view = new QTableView;
    _original_detector_pos_model = new QStandardItemModel(0, 4, this);
    _original_detector_pos_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    _original_detector_pos_view->setModel(_original_detector_pos_model);
    _original_detector_pos_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    _detector_pos_view = new QTableView;
    _detector_pos_model = new QStandardItemModel(0, 4, this);
    _detector_pos_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _detector_pos_view->setModel(_detector_pos_model);
    _detector_pos_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    detector_pos_layout->addWidget(_original_detector_pos_view);
    detector_pos_layout->addWidget(_detector_pos_view);
}

void RefinerTables::refreshDetectorPosTable(nsx::Refiner* refiner, nsx::DataSet* data)
{
    QStandardItemModel* m0 =
        dynamic_cast<QStandardItemModel*>(_original_detector_pos_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_detector_pos_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const nsx::InstrumentState& state : *refiner->unrefinedStates()) {
        QList<QStandardItem*> row;
        auto pos = state.detectorPositionOffset;
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(pos[0])));
        row.push_back(new QStandardItem(QString::number(pos[1])));
        row.push_back(new QStandardItem(QString::number(pos[2])));
        m0->appendRow(row);
        ++frame;
    }

    if (refiner == nullptr)
        return;

    if (data == nullptr)
        return;

    frame = 1;
    for (const nsx::InstrumentState& state : *refiner->refinedStates()) {
        QList<QStandardItem*> row;
        auto pos = state.detectorPositionOffset;
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(pos[0])));
        row.push_back(new QStandardItem(QString::number(pos[1])));
        row.push_back(new QStandardItem(QString::number(pos[2])));
        model->appendRow(row);
        ++frame;
    }
}

void RefinerTables::setKiTableUp()
{
    QHBoxLayout* ki_layout = new QHBoxLayout(_ki_tab);

    _original_ki_view = new QTableView;
    _original_ki_model = new QStandardItemModel(0, 4, this);
    _original_ki_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    _original_ki_view->setModel(_original_ki_model);
    _original_ki_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    _ki_view = new QTableView;
    _ki_model = new QStandardItemModel(0, 4, this);
    _ki_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _ki_view->setModel(_ki_model);
    _ki_model->setHorizontalHeaderLabels({"frame", "x", "y", "z"});
    ki_layout->addWidget(_original_ki_view);
    ki_layout->addWidget(_ki_view);
}

void RefinerTables::refreshKiTable(nsx::Refiner* refiner, nsx::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_ki_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_ki_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const nsx::InstrumentState& state : *refiner->unrefinedStates()) {
        QList<QStandardItem*> row;
        auto ki = state.ki().rowVector();
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(ki[0], 'g', 6)));
        row.push_back(new QStandardItem(QString::number(ki[1], 'g', 6)));
        row.push_back(new QStandardItem(QString::number(ki[2], 'g', 6)));
        m0->appendRow(row);
        ++frame;
    }

    if (refiner == nullptr)
        return;

    if (data == nullptr)
        return;

    frame = 1;
    for (const nsx::InstrumentState& state : *refiner->refinedStates()) {
        QList<QStandardItem*> row;
        auto ki = state.ki().rowVector();
        row.push_back(new QStandardItem(QString::number(frame)));
        row.push_back(new QStandardItem(QString::number(ki[0], 'g', 6)));
        row.push_back(new QStandardItem(QString::number(ki[1], 'g', 6)));
        row.push_back(new QStandardItem(QString::number(ki[2], 'g', 6)));
        model->appendRow(row);
        ++frame;
    }
}
