//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/frames/autoindexer.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/frames/autoindexer.h"
#include "nsxgui/gui/frames/unitcellwidget.h"
#include "nsxgui/gui/models/session.h"

#include "core/auto_indexing/AutoIndexer.h"
#include "core/utils/ProgressHandler.h"
#include "core/utils/Units.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QVBoxLayout>

AutoIndexer::AutoIndexer() : QcrFrame {"autoIndexer"}
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[WARNING] No experiment selected");
        return;
    }
    if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
        gLogger->log("[WARNING] No peaks in the selected experiment");
        return;
    }

    layout();
}

void AutoIndexer::layout()
{
    nsx::IndexerParameters indexParams;

    QVBoxLayout* whole = new QVBoxLayout(this);
    tabs = new QcrTabWidget("autoIndexerTabs");
    settings = new QcrWidget("Settings");
    QVBoxLayout* vertical = new QVBoxLayout(settings);
    model = new PeaksTableModel(
        "adhoc_autoIndexerPeakTable", gSession->selectedExperiment()->experiment(),
        gSession->selectedExperiment()->peaks()->allPeaks());
    peaks = new PeaksTableView;
    peaks->setModel(model);
    vertical->addWidget(peaks);
    QHBoxLayout* horizontal = new QHBoxLayout();
    QGroupBox* params = new QGroupBox("Parameters", this);
    QGridLayout* grid = new QGridLayout(params);
    grid->addWidget(new QLabel("Gruber tolerance"), 0, 0, 1, 1);
    grid->addWidget(new QLabel("Niggli tolerance"), 1, 0, 1, 1);
    only_niggli = new QcrCheckBox(
        "adhoc_autoIndexerOnlyNiggli", "find Niggli cell only", new QcrCell<bool>(false));
    grid->addWidget(only_niggli, 2, 0, 1, 1);
    grid->addWidget(new QLabel("maximum cell dimension"), 3, 0, 1, 1);
    grid->addWidget(new QLabel("number Q vertices"), 4, 0, 1, 1);
    grid->addWidget(new QLabel("number subdiversions"), 6, 0, 1, 1);
    grid->addWidget(new QLabel("number solutions"), 5, 0, 1, 1);
    grid->addWidget(new QLabel("minimum cell volume"), 7, 0, 1, 1);
    grid->addWidget(new QLabel("indexing tolerance"), 8, 0, 1, 1);
    gruber = new QcrDoubleSpinBox(
        "adhoc_autoIndexerGruber", new QcrCell<double>(indexParams.gruberTolerance), 8, 6);
    niggli = new QcrDoubleSpinBox(
        "adhoc_autoIndexerNiggli", new QcrCell<double>(indexParams.niggliTolerance), 8, 6);
    maxCellDim = new QcrDoubleSpinBox(
        "adhoc_autoIndexerMaxCellDim", new QcrCell<double>(indexParams.maxdim), 6, 2);
    nVertices =
        new QcrSpinBox("adhoc_autoIndexerVertices", new QcrCell<int>(indexParams.nVertices), 4);
    nSolutions =
        new QcrSpinBox("adhoc_autoIndexerolutions", new QcrCell<int>(indexParams.nSolutions), 4);
    nSubdivisions =
        new QcrSpinBox("adhoc_autoIndexerSubdiversion", new QcrCell<int>(indexParams.subdiv), 4);
    minCellVolume = new QcrDoubleSpinBox(
        "adhoc_autoIndexerMinCellVolume", new QcrCell<double>(indexParams.minUnitCellVolume), 5, 2);
    indexingTolerance = new QcrDoubleSpinBox(
        "adhoc_autoIndexingTol", new QcrCell<double>(indexParams.indexingTolerance), 5, 3);
    grid->addWidget(gruber, 0, 1, 1, 1);
    grid->addWidget(niggli, 1, 1, 1, 1);
    grid->addWidget(maxCellDim, 3, 1, 1, 1);
    grid->addWidget(nVertices, 4, 1, 1, 1);
    grid->addWidget(nSolutions, 5, 1, 1, 1);
    grid->addWidget(nSubdivisions, 6, 1, 1, 1);
    grid->addWidget(minCellVolume, 7, 1, 1, 1);
    grid->addWidget(indexingTolerance, 8, 1, 1, 1);
    horizontal->addWidget(params);
    solutions = new QTableView(this);
    horizontal->addWidget(solutions);
    vertical->addLayout(horizontal);
    tabs->addTab(settings, "Settings");
    whole->addWidget(tabs);
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
            | QDialogButtonBox::Reset,
        Qt::Horizontal, this);
    whole->addWidget(buttons);

    // connections
    connect(tabs->tabBar(), &QTabBar::tabBarDoubleClicked, this, &AutoIndexer::slotTabEdited);
    connect(tabs, &QcrTabWidget::tabCloseRequested, this, &AutoIndexer::slotTabRemoved);
    connect(
        solutions->verticalHeader(), &QHeaderView::sectionDoubleClicked, this,
        &AutoIndexer::selectSolution);
    connect(buttons, &QDialogButtonBox::clicked, this, &AutoIndexer::slotActionClicked);

    show();
}

void AutoIndexer::slotTabRemoved(int index)
{
    auto unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabs->widget(index));
    if (!unit_cell_tab) {
        return;
    }

    tabs->removeTab(index);

    delete unit_cell_tab;
}

void AutoIndexer::slotTabEdited(int index)
{
    auto unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabs->widget(index));

    if (!unit_cell_tab) {
        return;
    }

    QInputDialog dialog(this);
    dialog.setLabelText("");
    dialog.setWindowTitle(tr("Set unit cell name"));
    auto pos = mapToGlobal(tabs->pos());

    int width(0);
    for (auto i = 0; i < index; ++i) {
        width += tabs->tabBar()->tabRect(index).width();
    }

    int height = tabs->tabBar()->tabRect(index).height();

    dialog.move(pos.x() + width, pos.y() + height);

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    QString unit_cell_name = dialog.textValue();
    if (unit_cell_name.isEmpty()) {
        return;
    }

    tabs->setTabText(index, unit_cell_name);
    unit_cell_tab->unitCell()->setName(unit_cell_name.toStdString());

    QModelIndex topleft_index = model->index(0, 0);
    QModelIndex bottomright_index = model->index(model->rowCount() - 1, model->columnCount() - 1);

    emit model->dataChanged(topleft_index, bottomright_index);
}

void AutoIndexer::slotActionClicked(QAbstractButton* button)
{
    auto button_role = buttons->standardButton(button);

    switch (button_role) {
    case QDialogButtonBox::StandardButton::Reset: {
        resetUnitCell();
        break;
    }
    case QDialogButtonBox::StandardButton::Apply: {
        run();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        resetUnitCell();
        close();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}

void AutoIndexer::resetUnitCell()
{
    // Restore for each peak the initial unit cell
    for (auto p : _defaults) {
        p.first->setUnitCell(p.second);
    }

    for (auto i = tabs->count() - 1; i > 0; i--) {

        auto tab = dynamic_cast<UnitCellWidget*>(tabs->widget(i));
        if (!tab) {
            continue;
        }
        tabs->removeTab(i);
        delete tab;
    }

    // Update the peak table view
    QModelIndex topLeft = model->index(0, 0);
    QModelIndex bottomRight = model->index(model->rowCount() - 1, model->columnCount() - 1);
    emit model->dataChanged(topLeft, bottomRight);
}

void AutoIndexer::accept()
{
    auto unit_cells_item = gSession->selectedExperiment()->unitCells();

    for (auto i = 0; i < tabs->count(); ++i) {
        auto unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabs->widget(i));
        if (!unit_cell_tab) {
            continue;
        }
        unit_cells_item->appendUnitCell(unit_cell_tab->unitCell());
    }

    // emit _experiment_item->model()->itemChanged(unit_cells_item);

    close();
}

void AutoIndexer::run()
{
    auto selection_model = peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    if (selected_rows.empty()) {
        gLogger->log("[ERROR] No peaks selected for auto-indexing");
        return;
    }

    // auto handler = std::make_shared<nsx::ProgressHandler>();
    std::shared_ptr<nsx::ProgressHandler> handler(new nsx::ProgressHandler());

    handler->setCallback([handler]() {
        auto log = handler->getLog();
        for (auto&& msg : log) {
            gLogger->log("[INFO] " + QString::fromStdString(msg));
        }
    });

    handler->log("Test log");

    nsx::AutoIndexer indexer(handler);

    // Clear the current solution list
    _solutions.clear();

    auto allPeaks = model->peaks();

    for (auto r : selected_rows) {
        indexer.addPeak(allPeaks[r.row()]);
    }

    nsx::IndexerParameters params;

    //  params.subdiv = nSubdivisions->value();
    //  //; <- this is in the apps FrameAutoIndexer, and maxCellDim is gotten twice,
    //  //     while minCellVolume not once...?
    //  params.maxdim = maxCellDim->value();
    //  params.nSolutions = nSolutions->value();
    //  params.indexingTolerance = indexingTolerance->value();
    //  params.nVertices = nVertices->value();
    //  params.niggliReduction = only_niggli->isChecked();
    //  params.niggliTolerance = niggli->value();
    //  params.gruberTolerance = gruber->value();
    //  //params.maxdim = maxCellDim->value();
    //  params.minUnitCellVolume = minCellVolume->value();

    try {
        indexer.autoIndex(params);
    } catch (const std::exception& e) {
        gLogger->log("[ERROR] AutoIndex: " + QString::fromStdString(e.what()));
        return;
    }
    _solutions = indexer.solutions();

    buildSolutionsTable();
}

void AutoIndexer::buildSolutionsTable()
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
    for (unsigned int i = 0; i < _solutions.size(); ++i) {
        auto& cell = _solutions[i].first;
        double quality = _solutions[i].second;

        auto ch = cell->character();
        auto sigma = cell->characterSigmas();

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
        QStandardItem* col7 = new QStandardItem(QString::number(cell->volume(), 'f', 3));
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
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
    solutions->setModel(model);
}

void AutoIndexer::selectSolution(int index)
{
    auto selected_unit_cell = _solutions[index].first;

    auto selection_model = peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    selected_unit_cell->setName("new unit cell");

    auto allPeaks = model->peaks();

    for (auto r : selected_rows) {
        allPeaks[r.row()]->setUnitCell(selected_unit_cell);
    }

    UnitCellWidget* widget_unit_cell =
        new UnitCellWidget(selected_unit_cell, QString::fromStdString(selected_unit_cell->name()));
    tabs->addTab(widget_unit_cell, QString::fromStdString(selected_unit_cell->name()));
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setChecked(true);
    tabs->tabBar()->setTabButton(tabs->count() - 1, QTabBar::LeftSide, checkbox);

    QModelIndex topLeft = model->index(0, 0);
    QModelIndex bottomRight = model->index(model->rowCount() - 1, model->columnCount() - 1);
    emit model->dataChanged(topLeft, bottomRight);
}
