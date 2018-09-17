#include <string>

#include <QAbstractButton>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QShortcut>
#include <QStandardItemModel>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "CollectedPeaksModel.h"
#include "DialogAutoIndexing.h"
#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "PeakTableView.h"
#include "UnitCellItem.h"
#include "UnitCellsItem.h"

#include "ui_DialogAutoIndexing.h"

#include <QDebug>

DialogAutoIndexing* DialogAutoIndexing::_instance = nullptr;

DialogAutoIndexing* DialogAutoIndexing::create(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget* parent)
{
    if (!_instance) {
        _instance = new DialogAutoIndexing(experiment_item, peaks, parent);
    }

    return _instance;
}

DialogAutoIndexing* DialogAutoIndexing::Instance()
{
    return _instance;
}

DialogAutoIndexing::DialogAutoIndexing(ExperimentItem* experiment_item, const nsx::PeakList& peaks, QWidget *parent)
: QDialog(parent),
  _ui(new Ui::DialogAutoIndexing),
  _experiment_item(experiment_item)
{
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    _ui->niggli->setStyleSheet("font-weight: normal;");
    _ui->niggli->setCheckable(true);
    _ui->niggli->setChecked(false);

    // Accept solution and set Unit-Cell
    connect(_ui->solutions->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));

    // get set of default values to populate controls
    nsx::IndexerParameters params;

    _ui->gruberTolerance->setMinimum(0);
    _ui->gruberTolerance->setMaximum(100);
    _ui->gruberTolerance->setValue(params.gruberTolerance);

    _ui->maxCellDim->setMinimum(0);
    _ui->maxCellDim->setMaximum(9999);
    _ui->maxCellDim->setValue(params.maxdim);

    _ui->nSolutions->setMinimum(3);
    _ui->nSolutions->setMaximum(9999);
    _ui->nSolutions->setValue(params.nSolutions);

    _ui->nVertices->setMinimum(100);
    _ui->nVertices->setMaximum(999999);
    _ui->nVertices->setValue(params.nVertices);

    _ui->subdiv->setMinimum(0);
    _ui->subdiv->setMaximum(999999);
    _ui->subdiv->setValue(params.subdiv);

    _peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _ui->peaks->setModel(_peaks_model);

    _defaults.reserve(peaks.size());
    for (auto peak : peaks) {
        auto unit_cell = peak->unitCell();
        if (unit_cell) {
            _defaults.push_back(std::make_pair(peak,std::make_shared<nsx::UnitCell>(nsx::UnitCell(*unit_cell))));
        } else {
            _defaults.push_back(std::make_pair(peak,nullptr));
        }
    }

    QShortcut *delete_unit_cell_shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), _ui->unitCells);
    connect(delete_unit_cell_shortcut, SIGNAL(activated()), this, SLOT(removeUnitCells()));

    connect(_ui->unitCells,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(slotEditUnitCellName(QListWidgetItem*)));

    connect(_ui->cancelOK,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));
}

DialogAutoIndexing::~DialogAutoIndexing()
{
    if (_peaks_model) {
        delete _peaks_model;
    }

    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void DialogAutoIndexing::slotEditUnitCellName(QListWidgetItem *item)
{
    auto unit_cell = item->data(Qt::UserRole).value<nsx::sptrUnitCell>();
    unit_cell->setName(item->text().toStdString());

    QModelIndex topleft_index = _peaks_model->index(0,0);
    QModelIndex bottomright_index = _peaks_model->index(_peaks_model->rowCount(QModelIndex())-1,_peaks_model->columnCount(QModelIndex())-1);

    emit _peaks_model->dataChanged(topleft_index,bottomright_index);
}

void DialogAutoIndexing::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->cancelOK->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Reset: {
        resetUnitCell();
        break;
    }
    case QDialogButtonBox::StandardButton::Apply: {
        autoIndex();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        reject();
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

void DialogAutoIndexing::resetUnitCell()
{
    // Restore for each peak the initial unit cell
    for (auto p : _defaults) {
        p.first->setUnitCell(p.second);
    }

    // Update the peak table view
    QModelIndex topLeft = _peaks_model->index(0, 0);
    QModelIndex bottomRight = _peaks_model->index(_peaks_model->rowCount(QModelIndex())-1, _peaks_model->columnCount(QModelIndex())-1);
    emit _peaks_model->dataChanged(topLeft,bottomRight);

    // Clear the unit cell list
    _ui->unitCells->clear();
}

void DialogAutoIndexing::reject()
{
    resetUnitCell();

    QDialog::reject();
}

void DialogAutoIndexing::accept()
{
    auto unit_cells_item = _experiment_item->unitCellsItem();

    for (int i = 0; i < _ui->unitCells->count(); ++i) {
        auto item = _ui->unitCells->item(i);
        auto&& unit_cell = item->data(Qt::UserRole).value<nsx::sptrUnitCell>();
        unit_cell->setName(item->text().toStdString());
        auto unit_cell_item = new UnitCellItem(unit_cell);
        unit_cells_item->appendRow(unit_cell_item);
    }

    emit _experiment_item->model()->itemChanged(unit_cells_item);

    QDialog::accept();
}

void DialogAutoIndexing::removeUnitCells()
{
    auto selected_items = _ui->unitCells->selectedItems();

    for (auto item : selected_items) {
        _ui->unitCells->removeItemWidget(item);
        delete item;
    }
}

void DialogAutoIndexing::autoIndex()
{
    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    if (selected_rows.empty()) {
        nsx::error()<<"No peaks selected for auto-indexing";
        return;
    }

    auto handler = std::make_shared<nsx::ProgressHandler>();

    handler->setCallback([=]() {
       auto log = handler->getLog();
       for (auto&& msg: log) {
           nsx::info() << msg.c_str();
       }
    });

    nsx::AutoIndexer indexer(handler);

    // Clear the current solution list
    _solutions.clear();

    auto peaks = _peaks_model->peaks();

    for (auto r : selected_rows) {
        indexer.addPeak(peaks[r.row()]);
    }

    nsx::IndexerParameters params;

    params.subdiv = _ui->subdiv->value();;
    params.maxdim = _ui->maxCellDim->value();
    params.nSolutions = _ui->nSolutions->value();
    params.indexingTolerance = _ui->indexingTolerance->value();
    params.nVertices = _ui->nVertices->value();
    params.niggliReduction = _ui->niggli->isChecked();
    params.niggliTolerance = _ui->niggliTolerance->value();
    params.gruberTolerance = _ui->gruberTolerance->value();
    params.maxdim = _ui->maxCellDim->value();

    try {
        indexer.autoIndex(params);
    } catch (const std::exception& e) {
        nsx::error() << "AutoIndex: " << e.what();
        return;
    }
    _solutions = indexer.solutions();

    buildSolutionsTable();
}

void DialogAutoIndexing::buildSolutionsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(),9,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("a"));
    model->setHorizontalHeaderItem(1,new QStandardItem("b"));
    model->setHorizontalHeaderItem(2,new QStandardItem("c"));
    model->setHorizontalHeaderItem(3,new QStandardItem(QString((QChar) 0x03B1)));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03B2)));
    model->setHorizontalHeaderItem(5,new QStandardItem(QString((QChar) 0x03B3)));
    model->setHorizontalHeaderItem(6,new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7,new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8,new QStandardItem("Quality"));

    using nsx::deg;

    // Display solutions
    for (unsigned int i=0;i<_solutions.size();++i) {
        auto& cell=_solutions[i].first;
        double quality=_solutions[i].second;

        nsx::CellCharacter ch = cell->character();
        nsx::CellCharacter sigma = cell->characterSigmas();

        QStandardItem* col1 = new QStandardItem(QString::number(ch.a,'f',3) + "("+ QString::number(sigma.a*1000,'f',0)+")");
        QStandardItem* col2 = new QStandardItem(QString::number(ch.b,'f',3) + "("+ QString::number(sigma.b*1000,'f',0)+")");
        QStandardItem* col3 = new QStandardItem(QString::number(ch.c,'f',3) + "("+ QString::number(sigma.c*1000,'f',0)+")");
        QStandardItem* col4 = new QStandardItem(QString::number(ch.alpha/deg,'f',3)+ "("+ QString::number(sigma.alpha/deg*1000,'f',0)+")");
        QStandardItem* col5 = new QStandardItem(QString::number(ch.beta/deg,'f',3)+"("+ QString::number(sigma.beta/deg*1000,'f',0)+")");
        QStandardItem* col6 = new QStandardItem(QString::number(ch.gamma/deg,'f',3)+ "("+ QString::number(sigma.gamma/deg*1000,'f',0)+")");
        QStandardItem* col7 = new QStandardItem(QString::number(cell->volume(),'f',3));
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
        QStandardItem* col9 = new QStandardItem(QString::number(quality,'f',2)+"%");

        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i,2,col3);
        model->setItem(i,3,col4);
        model->setItem(i,4,col5);
        model->setItem(i,5,col6);
        model->setItem(i,6,col7);
        model->setItem(i,7,col8);
        model->setItem(i,8,col9);
    }
    _ui->solutions->setModel(model);
}

void DialogAutoIndexing::selectSolution(int index)
{
    auto selected_unit_cell = _solutions[index].first;

    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    selected_unit_cell->setName("new unit cell");

    auto peaks = _peaks_model->peaks();

    for (auto r : selected_rows) {
        peaks[r.row()]->setUnitCell(selected_unit_cell);
    }
    QModelIndex topLeft = _peaks_model->index(0, 0);
    QModelIndex bottomRight = _peaks_model->index(_peaks_model->rowCount(QModelIndex())-1, _peaks_model->columnCount(QModelIndex())-1);
    emit _peaks_model->dataChanged(topLeft,bottomRight);

    QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(selected_unit_cell->name()));
    item->setData(Qt::UserRole,QVariant::fromValue(selected_unit_cell));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    _ui->unitCells->addItem(item);
}
