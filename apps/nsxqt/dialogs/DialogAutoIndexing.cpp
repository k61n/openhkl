#include "dialogs/DialogAutoIndexing.h"
#include "ui_DialogAutoIndexing.h"

#include <map>
#include <string>

#include <QMenu>

#include <Eigen/Dense>

#include <QItemSelectionModel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QtDebug>

#include <nsxlib/crystal/AutoIndexer.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/utils/ProgressHandler.h>
#include "models/CollectedPeaksModel.h"
#include "models/CollectedPeaksDelegate.h"

using namespace nsx;

DialogAutoIndexing::DialogAutoIndexing(std::shared_ptr<Experiment> experiment, std::vector<sptrPeak3D> peaks, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogAutoIndexing),
    _experiment(experiment),
    _peaks(peaks)
{
    ui->setupUi(this);
    setModal(true);
    _unitCells = _experiment->getDiffractometer()->getSample()->getUnitCells();

    for (auto uc : _unitCells) {
        ui->unitCells->addItem(QString::fromStdString(uc->getName()));
    }
    connect(ui->index,SIGNAL(clicked()),this,SLOT(autoIndex()));

    // Accept solution and set Unit-Cell
    connect(ui->solutions->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectSolution(int)));
}

DialogAutoIndexing::~DialogAutoIndexing()
{
    delete ui;
}

void DialogAutoIndexing::autoIndex()
{
    auto handler = std::make_shared<ProgressHandler>();

    handler->setCallback([=]() {
       auto log = handler->getLog();
       for (auto&& msg: log) {
           qDebug() << msg.c_str();
       }
    });

    AutoIndexer indexer(_experiment, handler);

    if (_unitCells.empty()) {
        throw std::runtime_error("cannot auto index: no unit cell");
    }

    sptrUnitCell selectedUnitCell = _unitCells[ui->unitCells->currentIndex()];

    // Clear the current solution list
    _solutions.clear();


    for (auto peak : _peaks) {
        indexer.addPeak(peak);
    }

    AutoIndexer::Parameters params;

    params.subdiv = 5;
    params.maxdim = ui->maxCellDim->value();
    params.nSolutions = ui->maxNumSolutions->value();
    params.nStacks = ui->nStacks->value();
    params.HKLTolerance = selectedUnitCell->getHKLTolerance();

    params.niggliReduction = ui->niggliReduction->isChecked();
    params.niggliTolerance = ui->niggliTolerance->value();
    params.gruberTolerance = ui->gruberTolerance->value();

    if (indexer.autoIndex(params) == false) {
        qDebug() << "ERROR: failed to auto index!";
        return;
    }
    _solutions = indexer.getSolutions();

    for (auto&& sol: _solutions) {
        sol.first.setName(selectedUnitCell->getName());
    }

    buildSolutionsTable();
}

void DialogAutoIndexing::buildSolutionsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model=new QStandardItemModel(_solutions.size(),9,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("a"));
    model->setHorizontalHeaderItem(1,new QStandardItem("b"));
    model->setHorizontalHeaderItem(2,new QStandardItem("c"));
    model->setHorizontalHeaderItem(3,new QStandardItem(QString((QChar) 0x03B1)));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03B2)));
    model->setHorizontalHeaderItem(5,new QStandardItem(QString((QChar) 0x03B3)));
    model->setHorizontalHeaderItem(6,new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(8,new QStandardItem("Quality"));

    // Display solutions
    for (unsigned int i=0;i<_solutions.size();++i) {
        auto& cell=_solutions[i].first;
        double quality=_solutions[i].second;
        double a,b,c,alpha,beta,gamma;
        double sa,sb,sc,salpha,sbeta,sgamma;
        cell.getParameters(a,b,c,alpha,beta,gamma);
        cell.getParametersSigmas(sa,sb,sc,salpha,sbeta,sgamma);
        QStandardItem* col1=new QStandardItem(QString::number(a,'f',3) + "("+ QString::number(sa*1000,'f',0)+")");
        QStandardItem* col2=new QStandardItem(QString::number(b,'f',3) + "("+ QString::number(sb*1000,'f',0)+")");
        QStandardItem* col3=new QStandardItem(QString::number(c,'f',3) + "("+ QString::number(sc*1000,'f',0)+")");
        QStandardItem* col4=new QStandardItem(QString::number(alpha/deg,'f',3)+ "("+ QString::number(salpha/deg*1000,'f',0)+")");
        QStandardItem* col5=new QStandardItem(QString::number(beta/deg,'f',3)+"("+ QString::number(sbeta/deg*1000,'f',0)+")");
        QStandardItem* col6=new QStandardItem(QString::number(gamma/deg,'f',3)+ "("+ QString::number(sgamma/deg*1000,'f',0)+")");
        QStandardItem* col7=new QStandardItem(QString::number(cell.getVolume(),'f',3));
        QStandardItem* col8=new QStandardItem(QString::fromStdString(cell.getBravaisTypeSymbol()));
        QStandardItem* col9=new QStandardItem(QString::number(quality,'f',2)+"%");
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
    *_unitCells[ui->unitCells->currentIndex()] = _solutions[index].first;
    QString solutionNumber = QString::number(index+1);
    QString selectedUnitCellName = ui->unitCells->currentText();
    QMessageBox::information(this, tr("NSXTool"),tr("Solution %1 set to %2 unit cell").arg(solutionNumber,selectedUnitCellName));
    emit cellUpdated(_unitCells[ui->unitCells->currentIndex()]);
}
