//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/UserDefinedUnitCellIndexerFrame.cpp
//! @brief     Implements class UserDefinedUnitCellIndexerFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/UserDefinedUnitCellIndexerFrame.h"

#include "base/geometry/ReciprocalVector.h"

#include "base/utils/Units.h"
#include "core/algo/UserDefinedIndexer.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Source.h"
#include "core/peak/Peak3D.h"
#include "gui/frames/UnitCellWidget.h"
#include "gui/models/Session.h"
#include <QCR/engine/logger.h>
#include <QtGlobal>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

UserDefinedUnitCellIndexerFrame::UserDefinedUnitCellIndexerFrame()
    : QcrFrame{"adhoc_userDefined"}, indexer_{}
{
    setAttribute(Qt::WA_DeleteOnClose);

    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[WARNING] No experiment selected");
        return;
    }

    if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
        gLogger->log("[WARNING] No peaks in selected experiment");
        return;
    }

    layout();
}

void UserDefinedUnitCellIndexerFrame::layout()
{
    // defaults_
    nsx::PeakList peaks = gSession->selectedExperiment()->peaks()->allPeaks();
    defaults_.reserve(peaks.size());
    for (nsx::sptrPeak3D peak : peaks) {
        nsx::sptrUnitCell unit_cell = peak->unitCell();
        if (unit_cell)
            defaults_.push_back(std::make_pair(peak, std::make_shared<nsx::UnitCell>(*unit_cell)));
        else
            defaults_.push_back(std::make_pair(peak, nullptr));
    }
    // layout
    QVBoxLayout* whole = new QVBoxLayout(this);
    tabwidget = new QcrTabWidget("adhoc_unitCellIndexerTabs");
    tab = new QcrWidget("adhoc_tab");
    QVBoxLayout* settings = new QVBoxLayout(tab);
    PeaksTableModel* model = new PeaksTableModel(
        "adhoc_tablePeaks", gSession->selectedExperiment()->experiment(),
        gSession->selectedExperiment()->peaks()->allPeaks());
    peaktable = new PeaksTableView;
    peaktable->setModel(model);
    peaktable->selectAll();
    settings->addWidget(peaktable);
    QGroupBox* parameters = new QGroupBox("Parameters", tab);
    QVBoxLayout* params = new QVBoxLayout(parameters);
    QGroupBox* unitcell = new QGroupBox("Unit Cell", parameters);
    QGridLayout* cellgrid = new QGridLayout(unitcell);
    // Labels in UnitCell GroupBox
    cellgrid->addWidget(new QLabel("a (" + QString(QChar(0x212B)) + ")"), 0, 0, 1, 1);
    cellgrid->addWidget(new QLabel("b (" + QString(QChar(0x212B)) + ")"), 0, 2, 1, 1);
    cellgrid->addWidget(new QLabel("c (" + QString(QChar(0x212B)) + ")"), 0, 4, 1, 1);
    cellgrid->addWidget(
        new QLabel(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")"), 1, 0, 1, 1);
    cellgrid->addWidget(
        new QLabel(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")"), 1, 2, 1, 1);
    cellgrid->addWidget(
        new QLabel(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")"), 1, 4, 1, 1);
    cellgrid->addWidget(new QLabel("Wavelength (" + QString(QChar(0x212B)) + ")"), 2, 0, 1, 1);
    cellgrid->addWidget(new QLabel("Gruber tolerance"), 3, 0, 1, 1);
    cellgrid->addWidget(new QLabel("Niggli tolerance"), 3, 2, 1, 1);
    // DoubleSpinBoxes
    a = new QcrDoubleSpinBox("adhoc_a", new QcrCell<double>(0.0), 7, 4);
    b = new QcrDoubleSpinBox("adhoc_b", new QcrCell<double>(0.0), 7, 4);
    c = new QcrDoubleSpinBox("adhoc_c", new QcrCell<double>(0.0), 7, 4);
    alpha = new QcrDoubleSpinBox("adhoc_alpha", new QcrCell<double>(0.0), 7, 4);
    beta = new QcrDoubleSpinBox("adhoc_beta", new QcrCell<double>(0.0), 7, 4);
    gamma = new QcrDoubleSpinBox("adhoc_gamma", new QcrCell<double>(0.0), 7, 4);
    wavelength = new QcrDoubleSpinBox("adhoc_wavelength", new QcrCell<double>(0.0), 7, 3);
    gruber = new QcrDoubleSpinBox("adhoc_gruber", new QcrCell<double>(0.0), 10, 6);
    niggli = new QcrDoubleSpinBox("adhoc_niggli", new QcrCell<double>(0.0), 10, 6);
    cellgrid->addWidget(a, 0, 1, 1, 1);
    cellgrid->addWidget(b, 0, 3, 1, 1);
    cellgrid->addWidget(c, 0, 5, 1, 1);
    cellgrid->addWidget(alpha, 1, 1, 1, 1);
    cellgrid->addWidget(beta, 1, 3, 1, 1);
    cellgrid->addWidget(gamma, 1, 5, 1, 1);
    cellgrid->addWidget(wavelength, 2, 1, 1, 1);
    cellgrid->addWidget(gruber, 3, 1, 1, 1);
    cellgrid->addWidget(niggli, 3, 3, 1, 1);
    onlyNiggli = new QcrCheckBox("adhoc_only", "Niggli cell only", new QcrCell<bool>(false));
    cellgrid->addWidget(onlyNiggli, 3, 4, 1, 1);
    params->addWidget(unitcell);

    QGroupBox* indexing = new QGroupBox("Indexing", parameters);
    QGridLayout* indexgrid = new QGridLayout(indexing);
    // labels in Indexing GroupBox
    indexgrid->addWidget(new QLabel("Distance tolerance"), 0, 0, 1, 1);
    indexgrid->addWidget(new QLabel("Angular tolerance"), 0, 2, 1, 1);
    indexgrid->addWidget(new QLabel("Max Q vectors"), 0, 4, 1, 1);
    indexgrid->addWidget(new QLabel("Indexing tolerance"), 1, 0, 1, 1);
    indexgrid->addWidget(new QLabel("Indexing threshold"), 1, 2, 1, 1);
    indexgrid->addWidget(new QLabel("Number of solutions"), 1, 4, 1, 1);
    // DoubleSpinBoxes
    distance = new QcrDoubleSpinBox("adhoc_distanceTolerance", new QcrCell<double>(0.0), 8, 3);
    angularTol = new QcrDoubleSpinBox("adhoc_angular", new QcrCell<double>(0.0), 8, 3);
    indexingTol = new QcrDoubleSpinBox("adhoc_indexingTolerance", new QcrCell<double>(0.0), 8, 3);
    indexingThreshold = new QcrDoubleSpinBox("adhoc_threshold", new QcrCell<double>(0.0), 8, 3);
    indexgrid->addWidget(distance, 0, 1, 1, 1);
    indexgrid->addWidget(angularTol, 0, 3, 1, 1);
    indexgrid->addWidget(indexingTol, 1, 1, 1, 1);
    indexgrid->addWidget(indexingThreshold, 1, 3, 1, 1);
    maxQVectors = new QcrSpinBox("adhoc_qvectors", new QcrCell<int>(0), 6);
    solutions = new QcrSpinBox("adhoc_solutions", new QcrCell<int>(0), 6);
    indexgrid->addWidget(maxQVectors, 0, 5, 1, 1);
    indexgrid->addWidget(solutions, 1, 5, 1, 1);
    params->addWidget(indexing);
    settings->addWidget(parameters);
    view = new QTableView(this);
    settings->addWidget(view);
    tabwidget->addTab(tab, "Settings");

    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply
            | QDialogButtonBox::Reset,
        Qt::Horizontal, this);

    whole->addWidget(tabwidget);
    whole->addWidget(buttons);
    // parameters
    const nsx::UserDefinedUnitCellIndexerParameters& unitCellParameters = indexer_.parameters();
    a->setCellValue(unitCellParameters.a);
    alpha->setCellValue(unitCellParameters.alpha / nsx::deg);
    b->setCellValue(unitCellParameters.b);
    beta->setCellValue(unitCellParameters.beta / nsx::deg);
    c->setCellValue(unitCellParameters.c);
    gamma->setCellValue(unitCellParameters.gamma / nsx::deg);
    wavelength->setCellValue(gSession->selectedExperiment()
                                 ->experiment()
                                 ->diffractometer()
                                 ->source()
                                 .selectedMonochromator()
                                 .wavelength());
    gruber->setCellValue(unitCellParameters.gruber_tolerance);
    niggli->setCellValue(unitCellParameters.niggli_tolerance);
    onlyNiggli->setCellValue(unitCellParameters.niggli_only);
    distance->setCellValue(unitCellParameters.distance_tolerance);
    angularTol->setCellValue(unitCellParameters.angular_tolerance);
    indexingTol->setCellValue(unitCellParameters.indexing_tolerance);
    indexingThreshold->setCellValue(unitCellParameters.indexing_threshold);
    maxQVectors->setCellValue(unitCellParameters.max_n_q_vectors);
    solutions->setCellValue(unitCellParameters.n_solutions);
    // connections
    connect(
        tabwidget->tabBar(), &QTabBar::tabBarDoubleClicked, this,
        &UserDefinedUnitCellIndexerFrame::slotTabEdited);
    connect(
        tabwidget, &QcrTabWidget::tabCloseRequested, this,
        &UserDefinedUnitCellIndexerFrame::slotTabRemoved);

    connect(
        view->verticalHeader(), &QHeaderView::sectionDoubleClicked, this,
        &UserDefinedUnitCellIndexerFrame::slotSelectSolution);

    connect(
        buttons, &QDialogButtonBox::clicked, this,
        &UserDefinedUnitCellIndexerFrame::slotActionClicked);

    show();
}

void UserDefinedUnitCellIndexerFrame::slotActionClicked(QAbstractButton* button)
{
    auto button_role = buttons->standardButton(button);

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

void UserDefinedUnitCellIndexerFrame::slotSelectSolution(int index)
{
    nsx::sptrUnitCell selected_unit_cell = solutions_[index].first;

    QItemSelectionModel* selection_model = peaktable->selectionModel();

    QModelIndexList selected_rows = selection_model->selectedRows();

    selected_unit_cell->setName("new unit cell");

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(peaktable->model());

    const nsx::PeakList& peaks = peaks_model->peaks();

    for (QModelIndex r : selected_rows)
        peaks[r.row()]->setUnitCell(selected_unit_cell);

    UnitCellWidget* widget_unit_cell = new UnitCellWidget(
        selected_unit_cell, "adhoc_" + QString::fromStdString(selected_unit_cell->name()));
    tabwidget->addTab(widget_unit_cell, QString::fromStdString(selected_unit_cell->name()));
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setChecked(true);
    tabwidget->tabBar()->setTabButton(tabwidget->count() - 1, QTabBar::LeftSide, checkbox);

    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight =
        peaks_model->index(peaks_model->rowCount() - 1, peaks_model->columnCount() - 1);

    emit peaks_model->dataChanged(topLeft, bottomRight);
}

void UserDefinedUnitCellIndexerFrame::slotTabEdited(int index)
{
    UnitCellWidget* unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabwidget->widget(index));

    if (!unit_cell_tab)
        return;

    QInputDialog dialog(this);
    dialog.setLabelText("");
    dialog.setWindowTitle(tr("Set unit cell name"));
    QPoint pos = mapToGlobal(tabwidget->pos());

    int width(0);
    for (int i = 0; i < index; ++i)
        width += tabwidget->tabBar()->tabRect(index).width();

    int height = tabwidget->tabBar()->tabRect(index).height();

    dialog.move(pos.x() + width, pos.y() + height);

    if (dialog.exec() == QDialog::Rejected)
        return;

    QString unit_cell_name = dialog.textValue();
    if (unit_cell_name.isEmpty())
        return;

    tabwidget->setTabText(index, unit_cell_name);
    unit_cell_tab->unitCell()->setName(unit_cell_name.toStdString());

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(peaktable->model());

    QModelIndex topleft_index = peaks_model->index(0, 0);
    QModelIndex bottomright_index =
        peaks_model->index(peaks_model->rowCount() - 1, peaks_model->columnCount() - 1);

    emit peaks_model->dataChanged(topleft_index, bottomright_index);
}

void UserDefinedUnitCellIndexerFrame::slotTabRemoved(int index)
{
    UnitCellWidget* unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabwidget->widget(index));
    if (!unit_cell_tab)
        return;

    tabwidget->removeTab(index);

    delete unit_cell_tab;
}

void UserDefinedUnitCellIndexerFrame::resetPeaks()
{
    // Restore for each peak the initial unit cell
    for (auto p : defaults_)
        p.first->setUnitCell(p.second);

    for (int i = tabwidget->count() - 1; i > 0; i--) {
        UnitCellWidget* tab = dynamic_cast<UnitCellWidget*>(tabwidget->widget(i));
        if (!tab)
            continue;
        tabwidget->removeTab(i);
        delete tab;
    }

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(peaktable->model());

    // Update the peak table view
    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight =
        peaks_model->index(peaks_model->rowCount() - 1, peaks_model->columnCount() - 1);

    emit peaks_model->dataChanged(topLeft, bottomRight);
}

void UserDefinedUnitCellIndexerFrame::index()
{
    gLogger->log("[INFO] Begin indexing");
    QItemSelectionModel* selection_model = peaktable->selectionModel();

    QModelIndexList selected_rows = selection_model->selectedRows();

    if (selected_rows.empty()) {
        qWarning() << "No peaks selected for auto-indexing";
        return;
    }

    PeaksTableModel* peaks_model = dynamic_cast<PeaksTableModel*>(peaktable->model());
    const nsx::PeakList& peaks = peaks_model->peaks();

    nsx::PeakList selected_peaks;
    selected_peaks.reserve(selected_rows.size());
    for (QModelIndex r : selected_rows)
        selected_peaks.push_back(peaks[r.row()]);

    nsx::UserDefinedUnitCellIndexerParameters parameters;

    parameters.a = a->value();
    parameters.b = b->value();
    parameters.c = c->value();
    parameters.alpha = alpha->value() * nsx::deg;
    parameters.beta = beta->value() * nsx::deg;
    parameters.gamma = gamma->value() * nsx::deg;
    parameters.niggli_tolerance = niggli->value();
    parameters.gruber_tolerance = gruber->value();
    parameters.niggli_only = onlyNiggli->isChecked();
    parameters.n_solutions = solutions->value();
    parameters.indexing_tolerance = indexingTol->value();
    parameters.indexing_threshold = indexingThreshold->value();
    parameters.wavelength = wavelength->value();
    parameters.distance_tolerance = distance->value();
    parameters.angular_tolerance = angularTol->value();
    parameters.max_n_q_vectors = maxQVectors->value();
    indexer_.setParameters(parameters);
    indexer_.setPeaks(selected_peaks);
    gLogger->log("[INFO] Now running indexer...");
    indexer_.run();
    gLogger->log("[INFO] end running the indexer");
    solutions_ = indexer_.solutions();
    buildUnitCellsTable();
}

void UserDefinedUnitCellIndexerFrame::buildUnitCellsTable()
{
    // Create table with 9 columns
    gLogger->log("[INFO] build unit cells table");
    QStandardItemModel* model = new QStandardItemModel(solutions_.size(), 9, this);
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
    for (size_t i = 0; i < solutions_.size(); ++i) {
        nsx::sptrUnitCell unit_cell = solutions_[i].first;

        const double quality = solutions_[i].second;

        nsx::UnitCellCharacter ch = unit_cell->character();
        nsx::UnitCellCharacter sigma = unit_cell->characterSigmas();

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

    view->setModel(model);
}

void UserDefinedUnitCellIndexerFrame::accept()
{
    UnitCellsModel* unit_cells = gSession->selectedExperiment()->unitCells();

    for (int i = 0; i < tabwidget->count(); ++i) {
        UnitCellWidget* unit_cell_tab = dynamic_cast<UnitCellWidget*>(tabwidget->widget(i));
        if (!unit_cell_tab)
            continue;
        unit_cells->appendUnitCell(unit_cell_tab->unitCell());
    }

    // emit _experiment_item->model()->itemChanged(peaks_item);

    close();
}
