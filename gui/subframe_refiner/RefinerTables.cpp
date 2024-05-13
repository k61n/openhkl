//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/RefinerTables.cpp
//! @brief     Implements tables for SubframeRefiner
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_refiner/RefinerTables.h"

#include "base/utils/Units.h" // deg
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/instrument/InstrumentState.h"
#include "gui/widgets/PlotCheckBox.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QTableView>

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

    _nframes = 0;
}

void RefinerTables::refreshTables(ohkl::Refiner* refiner, ohkl::DataSet* data)
{
    _nframes = refiner->nframes();
    _x_vals.clear();
    for (int i = 0; i < _nframes; ++i)
        _x_vals.push_back(i);
    refreshLatticeTable(refiner);
    refreshSamplePosTable(refiner, data);
    refreshSampleOrnTable(refiner, data);
    refreshDetectorPosTable(refiner, data);
    refreshKiTable(refiner, data);
}

void RefinerTables::setLatticeTableUp()
{
    QHBoxLayout* lattice_layout = new QHBoxLayout(_lattice_tab);

    _original_lattice_view = new QTableView;
    _original_lattice_model = new QStandardItemModel(0, 7, this);
    _original_lattice_view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    _original_lattice_view->setModel(_original_lattice_model);
    _original_lattice_model->setHorizontalHeaderLabels(
        {"frame", "a", "b", "c", QString((QChar)0x03B1), QString((QChar)0x03B2),
         QString((QChar)0x03B3)});
    _lattice_view = new QTableView;
    _lattice_model = new QStandardItemModel(0, 7, this);
    _lattice_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _lattice_view->setModel(_lattice_model);
    _lattice_model->setHorizontalHeaderLabels(
        {"frame", "a", "b", "c", QString((QChar)0x03B1), QString((QChar)0x03B2),
         QString((QChar)0x03B3)});
    lattice_layout->addWidget(_original_lattice_view);
    lattice_layout->addWidget(_lattice_view);

    auto vscroll_unrefined = _original_lattice_view->verticalScrollBar();
    auto vscroll_refined = _lattice_view->verticalScrollBar();
    auto hscroll_unrefined = _original_lattice_view->horizontalScrollBar();
    auto hscroll_refined = _lattice_view->horizontalScrollBar();
    connect(vscroll_unrefined, SIGNAL(valueChanged(int)), vscroll_refined, SLOT(setValue(int)));
    connect(vscroll_refined, SIGNAL(valueChanged(int)), vscroll_unrefined, SLOT(setValue(int)));
    connect(hscroll_unrefined, SIGNAL(valueChanged(int)), hscroll_refined, SLOT(setValue(int)));
    connect(hscroll_refined, SIGNAL(valueChanged(int)), hscroll_unrefined, SLOT(setValue(int)));
}

void RefinerTables::refreshLatticeTable(ohkl::Refiner* refiner)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_lattice_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_lattice_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    for (int i = 0; i < refiner->nframes(); ++i) {
        QList<QStandardItem*> row;
        auto c = refiner->unrefinedCell()->character();
        row.push_back(new QStandardItem(QString::number(i)));
        row.push_back(new QStandardItem(QString::number(c.a)));
        row.push_back(new QStandardItem(QString::number(c.b)));
        row.push_back(new QStandardItem(QString::number(c.c)));
        row.push_back(new QStandardItem(QString::number(c.alpha / ohkl::deg)));
        row.push_back(new QStandardItem(QString::number(c.beta / ohkl::deg)));
        row.push_back(new QStandardItem(QString::number(c.gamma / ohkl::deg)));
        m0->appendRow(row);
    }

    if (!refiner)
        return;

    auto batches = refiner->batches();

    if (batches.empty())
        return;

    for (const auto& batch : batches) {
        for (int i = batch.first_frame(); i < batch.last_frame() - 2; ++i) {
            QList<QStandardItem*> row;
            auto c = batch.cell()->character();
            row.push_back(new QStandardItem(QString::number(i)));
            row.push_back(new QStandardItem(QString::number(c.a)));
            row.push_back(new QStandardItem(QString::number(c.b)));
            row.push_back(new QStandardItem(QString::number(c.c)));
            row.push_back(new QStandardItem(QString::number(c.alpha / ohkl::deg)));
            row.push_back(new QStandardItem(QString::number(c.beta / ohkl::deg)));
            row.push_back(new QStandardItem(QString::number(c.gamma / ohkl::deg)));
            model->appendRow(row);
        }
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

    auto scroll_unrefined = _original_sample_pos_view->verticalScrollBar();
    auto scroll_refined = _sample_pos_view->verticalScrollBar();
    connect(scroll_unrefined, SIGNAL(valueChanged(int)), scroll_refined, SLOT(setValue(int)));
    connect(scroll_refined, SIGNAL(valueChanged(int)), scroll_unrefined, SLOT(setValue(int)));
}

void RefinerTables::refreshSamplePosTable(ohkl::Refiner* refiner, ohkl::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_sample_pos_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_sample_pos_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const ohkl::InstrumentState& state : *refiner->unrefinedStates()) {
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
    for (const ohkl::InstrumentState& state : *refiner->refinedStates()) {
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
    _original_sample_orn_model->setHorizontalHeaderLabels(
        {"frame", "xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"});
    _sample_orn_view = new QTableView;
    _sample_orn_model = new QStandardItemModel(0, 10, this);
    _sample_orn_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _sample_orn_view->setModel(_sample_orn_model);
    _sample_orn_model->setHorizontalHeaderLabels(
        {"frame", "xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"});
    sample_orn_layout->addWidget(_original_sample_orn_view);
    sample_orn_layout->addWidget(_sample_orn_view);

    auto vscroll_unrefined = _original_sample_orn_view->verticalScrollBar();
    auto vscroll_refined = _sample_orn_view->verticalScrollBar();
    auto hscroll_unrefined = _original_sample_orn_view->horizontalScrollBar();
    auto hscroll_refined = _sample_orn_view->horizontalScrollBar();
    connect(vscroll_unrefined, SIGNAL(valueChanged(int)), vscroll_refined, SLOT(setValue(int)));
    connect(vscroll_refined, SIGNAL(valueChanged(int)), vscroll_unrefined, SLOT(setValue(int)));
    connect(hscroll_unrefined, SIGNAL(valueChanged(int)), hscroll_refined, SLOT(setValue(int)));
    connect(hscroll_refined, SIGNAL(valueChanged(int)), hscroll_unrefined, SLOT(setValue(int)));
}

void RefinerTables::refreshSampleOrnTable(ohkl::Refiner* refiner, ohkl::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_sample_orn_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_sample_orn_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const ohkl::InstrumentState& state : *refiner->unrefinedStates()) {
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
    for (const ohkl::InstrumentState& state : *refiner->refinedStates()) {
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

    auto scroll_unrefined = _original_detector_pos_view->verticalScrollBar();
    auto scroll_refined = _detector_pos_view->verticalScrollBar();
    connect(scroll_unrefined, SIGNAL(valueChanged(int)), scroll_refined, SLOT(setValue(int)));
    connect(scroll_refined, SIGNAL(valueChanged(int)), scroll_unrefined, SLOT(setValue(int)));
}

void RefinerTables::refreshDetectorPosTable(ohkl::Refiner* refiner, ohkl::DataSet* data)
{
    QStandardItemModel* m0 =
        dynamic_cast<QStandardItemModel*>(_original_detector_pos_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_detector_pos_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const ohkl::InstrumentState& state : *refiner->unrefinedStates()) {
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
    for (const ohkl::InstrumentState& state : *refiner->refinedStates()) {
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

    auto scroll_unrefined = _original_ki_view->verticalScrollBar();
    auto scroll_refined = _ki_view->verticalScrollBar();
    connect(scroll_unrefined, SIGNAL(valueChanged(int)), scroll_refined, SLOT(setValue(int)));
    connect(scroll_refined, SIGNAL(valueChanged(int)), scroll_unrefined, SLOT(setValue(int)));
}

void RefinerTables::refreshKiTable(ohkl::Refiner* refiner, ohkl::DataSet* data)
{
    QStandardItemModel* m0 = dynamic_cast<QStandardItemModel*>(_original_ki_view->model());
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_ki_view->model());
    m0->removeRows(0, m0->rowCount());
    model->removeRows(0, model->rowCount());

    int frame = 1;
    for (const ohkl::InstrumentState& state : *refiner->unrefinedStates()) {
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
    for (const ohkl::InstrumentState& state : *refiner->refinedStates()) {
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

QVector<double> RefinerTables::getXVals() const
{
    return _x_vals;
}

QVector<double> RefinerTables::getYVals(TableType table, int column) const
{
    QStandardItemModel* refined_model = nullptr;
    QStandardItemModel* unrefined_model = nullptr;
    switch (table) {
        case TableType::Lattice: {
            unrefined_model = _original_lattice_model;
            refined_model = _lattice_model;
            break;
        }
        case TableType::SamplePos: {
            unrefined_model = _original_sample_pos_model;
            refined_model = _sample_pos_model;
            break;
        }
        case TableType::SampleOrn: {
            unrefined_model = _original_sample_orn_model;
            refined_model = _sample_orn_model;
            break;
        }
        case TableType::DetectorPos: {
            unrefined_model = _original_detector_pos_model;
            refined_model = _detector_pos_model;
            break;
        }
        case TableType::Ki: {
            unrefined_model = _original_ki_model;
            refined_model = _ki_model;
            break;
        }
        default: break;
    }

    QVector<double> yvals;
    int ncols = refined_model->rowCount();
    for (int i = 0; i < _nframes; ++i) {
        double val0 = unrefined_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        if (i >= ncols) { // Todo (zamaan): better assigment of unit cells to frames?
            yvals.push_back(yvals.last());
            continue;
        }
        double val1 = refined_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        yvals.push_back(val1 - val0);
    }
    return yvals;
}
