#include <string>

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
#include "UnitCellItem.h"
#include "UnitCellsItem.h"

#include "ui_DialogAutoIndexing.h"

DialogAutoIndexing::DialogAutoIndexing(ExperimentItem* experiment_item, nsx::PeakList peaks, QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogAutoIndexing),
  _experiment_item(experiment_item),
  _peaks(peaks)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    ui->niggli->setStyleSheet("font-weight: normal;");
    ui->niggli->setCheckable(true);
    ui->niggli->setChecked(false);

    connect(ui->index,SIGNAL(clicked()),this,SLOT(autoIndex()));

    // Accept solution and set Unit-Cell
    connect(ui->solutions->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));

    // get set of default values to populate controls
    nsx::IndexerParameters params;

    ui->gruberTolerance->setMinimum(0);
    ui->gruberTolerance->setMaximum(100);
    ui->gruberTolerance->setValue(params.gruberTolerance);

    ui->maxCellDim->setMinimum(0);
    ui->maxCellDim->setMaximum(9999);
    ui->maxCellDim->setValue(params.maxdim);

    ui->nSolutions->setMinimum(3);
    ui->nSolutions->setMaximum(9999);
    ui->nSolutions->setValue(params.nSolutions);

    ui->nVertices->setMinimum(100);
    ui->nVertices->setMaximum(999999);
    ui->nVertices->setValue(params.nVertices);

    ui->subdiv->setMinimum(0);
    ui->subdiv->setMaximum(999999);
    ui->subdiv->setValue(params.subdiv);

    auto model = new CollectedPeaksModel(_experiment_item->experiment(),peaks);
    ui->peaks->setModel(model);
    ui->peaks->verticalHeader()->show();

    ui->peaks->hideColumn(CollectedPeaksModel::Column::transmission);
    ui->peaks->hideColumn(CollectedPeaksModel::Column::lorentzFactor);
    ui->peaks->hideColumn(CollectedPeaksModel::Column::selected);
    ui->peaks->hideColumn(CollectedPeaksModel::Column::unitCell);

    ui->peaks->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->peaks->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

    ui->unitCells->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->unitCells);
    connect(shortcut, SIGNAL(activated()), this, SLOT(removeUnitCells()));

    connect(ui->cancelOK,SIGNAL(rejected()),this,SLOT(reject()));
    connect(ui->cancelOK,SIGNAL(accepted()),this,SLOT(accept()));
}

DialogAutoIndexing::~DialogAutoIndexing()
{
    delete ui;
}

void DialogAutoIndexing::accept()
{
    auto unit_cells_item = _experiment_item->unitCellsItem();

    for (int i = 0; i < ui->unitCells->count(); ++i) {
        auto item = ui->unitCells->item(i);
        auto&& unit_cell = item->data(Qt::UserRole).value<nsx::sptrUnitCell>();
        unit_cell->setName(item->text().toStdString());
        unit_cells_item->appendRow(new UnitCellItem(unit_cell));
    }

    QDialog::accept();
}

void DialogAutoIndexing::removeUnitCells()
{
    auto selected_items = ui->unitCells->selectedItems();

    for (auto item : selected_items) {
        ui->unitCells->removeItemWidget(item);
        delete item;
    }
}

void DialogAutoIndexing::autoIndex()
{
    auto selection_model = ui->peaks->selectionModel();

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

    for (auto r : selected_rows) {
        indexer.addPeak(_peaks[r.row()]);
    }

    nsx::IndexerParameters params;

    params.subdiv = ui->subdiv->value();;
    params.maxdim = ui->maxCellDim->value();
    params.nSolutions = ui->nSolutions->value();
    params.indexingTolerance = ui->indexingTolerance->value();
    params.nVertices = ui->nVertices->value();
    params.niggliReduction = ui->niggli->isChecked();
    params.niggliTolerance = ui->niggliTolerance->value();
    params.gruberTolerance = ui->gruberTolerance->value();
    params.maxdim = ui->maxCellDim->value();

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
    ui->solutions->setModel(model);
}

void DialogAutoIndexing::selectSolution(int index)
{
    auto selected_unit_cell = _solutions[index].first;

    auto selection_model = ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    selected_unit_cell->setName("new unit cell");

    for (auto r : selected_rows) {
        _peaks[r.row()]->setUnitCell(selected_unit_cell);
    }

    QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(selected_unit_cell->name()));
    item->setData(Qt::UserRole,QVariant::fromValue(selected_unit_cell));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->unitCells->addItem(item);
}
