#include "include/DialogUnitCellSolutions.h"
#include "ui_dialogunitcellsolutions.h"
#include  "UnitCell.h"
#include <QStandardItem>
#include "Units.h"


using SX::Units::deg;

DialogUnitCellSolutions::DialogUnitCellSolutions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUnitCellSolutions)
{
    ui->setupUi(this);
    // Make sure that the user can not edit the content of the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cellin the table select the whole line.
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

DialogUnitCellSolutions::~DialogUnitCellSolutions()
{
    delete ui;
}

void DialogUnitCellSolutions::setSolutions(const std::vector<std::pair<SX::Crystal::UnitCell,double>> &solutions)
{
    // Create table with 5 columns
    QStandardItemModel* model=new QStandardItemModel(solutions.size(),3,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("Conventional cell"));
    model->setHorizontalHeaderItem(1,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(2,new QStandardItem("Quality"));

    // Setup content of the table
    int i=0;
    for (const auto& solution : solutions)
    {
        const SX::Crystal::UnitCell& cell=solution.first;
        double Quality=solution.second;
        QString lc=QString::number(cell.getA(),'f',3)+","+QString::number(cell.getB(),'f',3)+","+QString::number(cell.getC(),'f',3);
        lc+=","+QString::number(cell.getAlpha()/deg,'f',2)+","+QString::number(cell.getBeta()/deg,'f',2)+","+QString::number(cell.getGamma()/deg,'f',2);
        QStandardItem* col1=new QStandardItem(lc);
        QStandardItem* col2=new QStandardItem(QString::fromStdString(cell.getBravaisTypeSymbol()));
        QStandardItem* col3=new QStandardItem(QString::number(Quality,'f',2));
        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i++,2,col3);


    }
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0,200);
    connect(ui->tableView->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectLine(int)));
}

void DialogUnitCellSolutions::selectLine(int i)
{
    emit selectSolution(i);
}
