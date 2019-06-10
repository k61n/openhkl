//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameUserDefinedUnitCellIndexer.cpp
//! @brief     Implements class FrameUserDefinedUnitCellIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>

#include "core/indexing/UserDefinedIndexer.h"
#include "core/peak/Peak3D.h"
#include "core/experiment/Experiment.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"
#include "core/logger/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/utils/Units.h"

#include "apps/frames/FrameUserDefinedUnitCellIndexer.h"
#include "apps/models/CollectedPeaksModel.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/PeaksItem.h"
#include "apps/models/SessionModel.h"
#include "apps/models/UnitCellItem.h"
#include "apps/models/UnitCellsItem.h"
#include "apps/widgets/WidgetUnitCell.h"

#include "ui_FrameUserDefinedUnitCellIndexer.h"

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::_instance = nullptr;

FrameUserDefinedUnitCellIndexer*
FrameUserDefinedUnitCellIndexer::create(ExperimentItem* experiment_item, const nsx::PeakList& peaks)
{
    if (!_instance)
        _instance = new FrameUserDefinedUnitCellIndexer(experiment_item, peaks);

    return _instance;
}

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::Instance()
{
    return _instance;
}

FrameUserDefinedUnitCellIndexer::FrameUserDefinedUnitCellIndexer(
    ExperimentItem* experiment_item, const nsx::PeakList& peaks)
    : NSXQFrame()
    , _ui(new Ui::FrameUserDefinedUnitCellIndexer)
    , _experiment_item(experiment_item)
    , _indexer()
{
    _ui->setupUi(this);

    _ui->tabs->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    CollectedPeaksModel* peaks_model =
        new CollectedPeaksModel(_experiment_item->model(), _experiment_item->experiment(), peaks);
    _ui->peaks->setModel(peaks_model);
    _ui->peaks->selectAll();

    _ui->label_a->setText("a (" + QString(QChar(0x212B)) + ")");
    _ui->label_b->setText("b (" + QString(QChar(0x212B)) + ")");
    _ui->label_c->setText("c (" + QString(QChar(0x212B)) + ")");

    _ui->label_alpha->setText(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_beta->setText(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_gamma->setText(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")");

    _ui->label_wavelength->setText("Wavelength (" + QString(QChar(0x212B)) + ")");

    auto&& parameters = _indexer.parameters();

    _ui->a->setValue(parameters.a);
    _ui->b->setValue(parameters.b);
    _ui->c->setValue(parameters.c);

    _ui->alpha->setValue(parameters.alpha / nsx::deg);
    _ui->beta->setValue(parameters.beta / nsx::deg);
    _ui->gamma->setValue(parameters.gamma / nsx::deg);

    _ui->distance_tolerance->setValue(parameters.distance_tolerance);

    _ui->angular_tolerance->setValue(parameters.angular_tolerance);

    _ui->niggli_tolerance->setValue(parameters.niggli_tolerance);
    _ui->gruber_tolerance->setValue(parameters.gruber_tolerance);
    _ui->niggli_only->setChecked(parameters.niggli_only);

    _ui->n_solutions->setValue(parameters.n_solutions);
    _ui->indexing_tolerance->setValue(parameters.indexing_tolerance);
    _ui->indexing_threshold->setValue(parameters.indexing_threshold);

    _ui->wavelength->setValue(_experiment_item->experiment()
                                  ->diffractometer()
                                  ->source()
                                  .selectedMonochromator()
                                  .wavelength());

    _ui->max_n_q_vectors->setValue(parameters.max_n_q_vectors);

    _defaults.reserve(peaks.size());
    for (auto peak : peaks) {
        auto unit_cell = peak->unitCell();
        if (unit_cell)
            _defaults.push_back(std::make_pair(peak, std::make_shared<nsx::UnitCell>(*unit_cell)));
        else
            _defaults.push_back(std::make_pair(peak, nullptr));
    }

    connect(_ui->tabs->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this, SLOT(slotTabEdited(int)));
    connect(_ui->tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabRemoved(int)));

    connect(
        _ui->unit_cells->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this,
        SLOT(slotSelectSolution(int)));

    connect(
        _ui->actions, SIGNAL(clicked(QAbstractButton*)), this,
        SLOT(slotActionClicked(QAbstractButton*)));
}

FrameUserDefinedUnitCellIndexer::~FrameUserDefinedUnitCellIndexer()
{
    delete _ui;

    if (_instance)
        _instance = nullptr;
}

void FrameUserDefinedUnitCellIndexer::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch (button_role) {
    case QDialogButtonBox::StandardButton::Apply: {
        index();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        resetPeaks();
        close();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    case QDialogButtonBox::StandardButton::Reset: {
        resetPeaks();
        break;
    }
    default: {
        return;
    }
    }
}

void FrameUserDefinedUnitCellIndexer::slotSelectSolution(int index)
{
    auto selected_unit_cell = _solutions[index].first;

    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    selected_unit_cell->setName("new unit cell");

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());

    auto peaks = peaks_model->peaks();

    for (auto r : selected_rows)
        peaks[r.row()]->setUnitCell(selected_unit_cell);

    WidgetUnitCell* widget_unit_cell = new WidgetUnitCell(selected_unit_cell);
    _ui->tabs->addTab(widget_unit_cell, QString::fromStdString(selected_unit_cell->name()));
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setChecked(true);
    _ui->tabs->tabBar()->setTabButton(_ui->tabs->count() - 1, QTabBar::LeftSide, checkbox);

    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight = peaks_model->index(
        peaks_model->rowCount(QModelIndex()) - 1, peaks_model->columnCount(QModelIndex()) - 1);

    emit peaks_model->dataChanged(topLeft, bottomRight);
}

void FrameUserDefinedUnitCellIndexer::slotTabEdited(int index)
{
    auto unit_cell_tab = dynamic_cast<WidgetUnitCell*>(_ui->tabs->widget(index));

    if (!unit_cell_tab)
        return;

    QInputDialog dialog(this);
    dialog.setLabelText("");
    dialog.setWindowTitle(tr("Set unit cell name"));
    auto pos = mapToGlobal(_ui->tabs->pos());

    int width(0);
    for (auto i = 0; i < index; ++i)
        width += _ui->tabs->tabBar()->tabRect(index).width();

    int height = _ui->tabs->tabBar()->tabRect(index).height();

    dialog.move(pos.x() + width, pos.y() + height);

    if (dialog.exec() == QDialog::Rejected)
        return;

    QString unit_cell_name = dialog.textValue();
    if (unit_cell_name.isEmpty())
        return;

    _ui->tabs->setTabText(index, unit_cell_name);
    unit_cell_tab->unitCell()->setName(unit_cell_name.toStdString());

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());

    QModelIndex topleft_index = peaks_model->index(0, 0);
    QModelIndex bottomright_index = peaks_model->index(
        peaks_model->rowCount(QModelIndex()) - 1, peaks_model->columnCount(QModelIndex()) - 1);

    emit peaks_model->dataChanged(topleft_index, bottomright_index);
}

void FrameUserDefinedUnitCellIndexer::slotTabRemoved(int index)
{
    auto unit_cell_tab = dynamic_cast<WidgetUnitCell*>(_ui->tabs->widget(index));
    if (!unit_cell_tab)
        return;

    _ui->tabs->removeTab(index);

    delete unit_cell_tab;
}

void FrameUserDefinedUnitCellIndexer::resetPeaks()
{
    // Restore for each peak the initial unit cell
    for (auto p : _defaults)
        p.first->setUnitCell(p.second);

    for (auto i = _ui->tabs->count() - 1; i > 0; i--) {

        auto tab = dynamic_cast<WidgetUnitCell*>(_ui->tabs->widget(i));
        if (!tab)
            continue;
        _ui->tabs->removeTab(i);
        delete tab;
    }

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());

    // Update the peak table view
    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight = peaks_model->index(
        peaks_model->rowCount(QModelIndex()) - 1, peaks_model->columnCount(QModelIndex()) - 1);

    emit peaks_model->dataChanged(topLeft, bottomRight);
}

void FrameUserDefinedUnitCellIndexer::index()
{
    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    if (selected_rows.empty()) {
        nsx::error() << "No peaks selected for auto-indexing";
        return;
    }

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());
    auto peaks = peaks_model->peaks();

    nsx::PeakList selected_peaks;
    selected_peaks.reserve(selected_rows.size());
    for (auto r : selected_rows)
        selected_peaks.push_back(peaks[r.row()]);

    nsx::UserDefinedUnitCellIndexerParameters parameters;

    parameters.a = _ui->a->value();
    parameters.b = _ui->b->value();
    parameters.c = _ui->c->value();

    parameters.alpha = _ui->alpha->value() * nsx::deg;
    parameters.beta = _ui->beta->value() * nsx::deg;
    parameters.gamma = _ui->gamma->value() * nsx::deg;

    parameters.niggli_tolerance = _ui->niggli_tolerance->value();
    parameters.gruber_tolerance = _ui->gruber_tolerance->value();
    parameters.niggli_only = _ui->niggli_only->isChecked();

    parameters.n_solutions = _ui->n_solutions->value();
    parameters.indexing_tolerance = _ui->indexing_tolerance->value();
    parameters.indexing_threshold = _ui->indexing_threshold->value();

    parameters.wavelength = _ui->wavelength->value();

    parameters.distance_tolerance = _ui->distance_tolerance->value();

    parameters.angular_tolerance = _ui->angular_tolerance->value();

    parameters.max_n_q_vectors = _ui->max_n_q_vectors->value();

    _indexer.setParameters(parameters);

    _indexer.setPeaks(selected_peaks);

    _indexer.run();

    _solutions = _indexer.solutions();

    buildUnitCellsTable();
}

void FrameUserDefinedUnitCellIndexer::buildUnitCellsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(), 9, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("a"));
    model->setHorizontalHeaderItem(1, new QStandardItem("b"));
    model->setHorizontalHeaderItem(2, new QStandardItem("c"));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString((QChar)0x03B1)));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString((QChar)0x03B2)));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString((QChar)0x03B3)));
    model->setHorizontalHeaderItem(6, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7, new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8, new QStandardItem("Quality"));

    // Display solutions
    for (size_t i = 0; i < _solutions.size(); ++i) {

        auto&& unit_cell = _solutions[i].first;

        const double quality = _solutions[i].second;

        auto ch = unit_cell->character();
        auto sigma = unit_cell->characterSigmas();

        QStandardItem* col1 = new QStandardItem(
            QString::number(ch.a, 'f', 3) + "(" + QString::number(sigma.a * 1000, 'f', 0) + ")");
        QStandardItem* col2 = new QStandardItem(
            QString::number(ch.b, 'f', 3) + "(" + QString::number(sigma.b * 1000, 'f', 0) + ")");
        QStandardItem* col3 = new QStandardItem(
            QString::number(ch.c, 'f', 3) + "(" + QString::number(sigma.c * 1000, 'f', 0) + ")");
        QStandardItem* col4 = new QStandardItem(
            QString::number(ch.alpha / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.alpha / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col5 = new QStandardItem(
            QString::number(ch.beta / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.beta / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col6 = new QStandardItem(
            QString::number(ch.gamma / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.gamma / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col7 = new QStandardItem(QString::number(unit_cell->volume(), 'f', 3));
        QStandardItem* col8 =
            new QStandardItem(QString::fromStdString(unit_cell->bravaisTypeSymbol()));
        QStandardItem* col9 = new QStandardItem(QString::number(quality, 'f', 2) + "%");

        model->setItem(i, 0, col1);
        model->setItem(i, 1, col2);
        model->setItem(i, 2, col3);
        model->setItem(i, 3, col4);
        model->setItem(i, 4, col5);
        model->setItem(i, 5, col6);
        model->setItem(i, 6, col7);
        model->setItem(i, 7, col8);
        model->setItem(i, 8, col9);
    }

    _ui->unit_cells->setModel(model);
}

void FrameUserDefinedUnitCellIndexer::accept()
{
    auto unit_cells_item = _experiment_item->unitCellsItem();

    for (auto i = 0; i < _ui->tabs->count(); ++i) {
        auto unit_cell_tab = dynamic_cast<WidgetUnitCell*>(_ui->tabs->widget(i));
        if (!unit_cell_tab)
            continue;
        auto unit_cell_item = new UnitCellItem(unit_cell_tab->unitCell());
        unit_cells_item->appendRow(unit_cell_item);
    }

    auto peaks_item = _experiment_item->peaksItem();

    emit _experiment_item->model()->itemChanged(peaks_item);

    close();
}
