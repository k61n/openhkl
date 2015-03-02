#include "include/Chemistry/AddElementDialog.h"
#include "ui_AddElementDialog.h"
#include "IsotopeManager.h"
#include "include/Chemistry/DragElementModel.h"

#include <iostream>
#include <string>
#include <vector>

#include <QString>
#include <QStringList>

AddElementDialog::AddElementDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::AddElementDialog)
{
    ui->setupUi(this);

    // Set the isotope manager
    _isotopeMgr = SX::Chemistry::IsotopeManager::Instance();

    std::vector<std::string> isotopesName=_isotopeMgr->getDatabaseNames();

    for (const auto& isName : isotopesName)
        ui->isotopesList->addItem(QString::fromStdString(isName));

    ui->isotopesList->setDragEnabled(true);

    _model = new DragElementModel();

    ui->selectedIsotopesView->setModel(_model);
    ui->selectedIsotopesView->setAcceptDrops(true);

//    connect(_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dropIsotope(QModelIndex,QModelIndex)));

}

AddElementDialog::~AddElementDialog()
{
    delete ui;
}

void AddElementDialog::dropIsotope(QModelIndex, QModelIndex)
{
    std::cout<<"I have been called"<<std::endl;
}
