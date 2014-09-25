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
    // Create table with 9 columns
    QStandardItemModel* model=new QStandardItemModel(solutions.size(),9,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("a"));
    model->setHorizontalHeaderItem(1,new QStandardItem("b"));
    model->setHorizontalHeaderItem(2,new QStandardItem("c"));
    model->setHorizontalHeaderItem(3,new QStandardItem(QString((QChar) 0x03B1)));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03B2)));
    model->setHorizontalHeaderItem(5,new QStandardItem(QString((QChar) 0x03B3)));
    model->setHorizontalHeaderItem(6,new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(8,new QStandardItem("Quality"));

    // Setup content of the table
    int i=0;
    for (const auto& solution : solutions)
    {
        const SX::Crystal::UnitCell& cell=solution.first;
        double Quality=solution.second;

        QStandardItem* col1=new QStandardItem(QString::number(cell.getA(),'f',3));
        QStandardItem* col2=new QStandardItem(QString::number(cell.getB(),'f',3));
        QStandardItem* col3=new QStandardItem(QString::number(cell.getC(),'f',3));
        QStandardItem* col4=new QStandardItem(QString::number(cell.getAlpha()/deg,'f',2));
        QStandardItem* col5=new QStandardItem(QString::number(cell.getBeta()/deg,'f',2));
        QStandardItem* col6=new QStandardItem(QString::number(cell.getGamma()/deg,'f',2));
        QStandardItem* col7=new QStandardItem(QString::number(cell.getVolume()/deg,'f',3));
        QStandardItem* col8=new QStandardItem(QString::fromStdString(cell.getBravaisTypeSymbol()));
        QStandardItem* col9=new QStandardItem(QString::number(Quality,'f',2));
        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i,2,col3);
        model->setItem(i,3,col4);
        model->setItem(i,4,col5);
        model->setItem(i,5,col6);
        model->setItem(i,6,col7);
        model->setItem(i,7,col8);
        model->setItem(i++,8,col9);

    }
    ui->tableView->setModel(model);
    connect(ui->tableView->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(selectLine(int)));
}

void DialogUnitCellSolutions::selectLine(int i)
{
    emit selectSolution(i);
}
