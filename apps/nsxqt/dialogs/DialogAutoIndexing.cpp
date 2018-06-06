#include <map>
#include <string>

#include <QMenu>

#include <Eigen/Dense>

#include <QItemSelectionModel>
#include <QMessageBox>
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

#include "CollectedPeaksDelegate.h"
#include "CollectedPeaksModel.h"
#include "DialogAutoIndexing.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "MetaTypes.h"
#include "SampleItem.h"

#include "ui_DialogAutoIndexing.h"

DialogAutoIndexing::DialogAutoIndexing(ExperimentItem* experiment_item, nsx::PeakList peaks, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogAutoIndexing),
    _experiment_item(experiment_item),
    _peaks(peaks),
    _unitCell(nullptr)
{
    ui->setupUi(this);
    setModal(true);

    connect(ui->index,SIGNAL(clicked()),this,SLOT(autoIndex()));

    // Accept solution and set Unit-Cell
    connect(ui->solutions->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));

    connect(ui->cancelOK,SIGNAL(rejected()),this,SLOT(reject()));
    connect(ui->cancelOK,SIGNAL(accepted()),this,SLOT(accept()));
}

DialogAutoIndexing::~DialogAutoIndexing()
{
    delete ui;
}

void DialogAutoIndexing::autoIndex()
{
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

    for (auto peak : _peaks) {
        indexer.addPeak(peak);
    }

    nsx::IndexerParameters params;

    params.subdiv = 5;
    params.maxdim = ui->maxCellDim->value();
    params.nSolutions = ui->maxNumSolutions->value();
    params.indexingTolerance = ui->indexingTolerance->value();
    params.nVertices = ui->nVertices->value();
    params.niggliReduction = ui->niggliReduction->isChecked();
    params.niggliTolerance = ui->niggliTolerance->value();
    params.gruberTolerance = ui->gruberTolerance->value();

    try {
        indexer.autoIndex(params);
    } catch (...) {
        nsx::error() << "failed to auto index!";
        return;
    }
    _solutions = indexer.getSolutions();

    for (auto&& sol: _solutions) {
        sol.first->setName("new unit cell");
    }

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
    _unitCell = _solutions[index].first;
    _unitCell->setName("new unit cell");

    QMessageBox::information(this, tr("NSXTool"),tr("Solution %1 set").arg(QString::number(index+1)));
}

nsx::sptrUnitCell DialogAutoIndexing::unitCell()
{
    return _unitCell;
}
