#include "IsotopeManager.h"
#include "Element.h"
#include "ElementManager.h"
#include "Error.h"

#include "ui_AddElementDialog.h"
#include "include/Chemistry/AddElementDialog.h"
#include "include/Chemistry/DragElementModel.h"

#include <iostream>
#include <string>
#include <vector>

#include <QMessageBox>
#include <QString>
#include <QStringList>

AddElementDialog::AddElementDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::AddElementDialog)
{
    ui->setupUi(this);

    // Set the isotope manager
    _elementMgr = SX::Chemistry::ElementManager::Instance();

    // Set the isotope manager
    _isotopeMgr = SX::Chemistry::IsotopeManager::Instance();

    ui->elementsList->setDragEnabled(true);
    for (const auto& p : _elementMgr->getRegistry())
        ui->elementsList->addItem(QString::fromStdString(p.first));

    ui->isotopesList->setDragEnabled(true);
    for (const auto& isName : _isotopeMgr->getDatabaseNames())
        ui->isotopesList->addItem(QString::fromStdString(isName));

    _model = new DragElementModel();

    ui->selectedIsotopesView->setModel(_model);
    ui->selectedIsotopesView->setAcceptDrops(true);
}

AddElementDialog::~AddElementDialog()
{
    delete ui;
}

void AddElementDialog::on_cancelButton_clicked()
{
    this->destroy();
}

void AddElementDialog::on_saveButton_clicked()
{

    // Gets the element name
    QString elementName=ui->elementNameEntry->text().simplified();

    // Checks that it is not empty, otherwise return
    if (elementName.isEmpty())
    {
        QMessageBox::warning(this,"Add element","Empty element name.");
        return;
    }

    SX::Chemistry::ElementManager* emgr=SX::Chemistry::ElementManager::Instance();

    // Checks that is not already used in the elements database, otherwise return
    if (emgr->isInDatabase(elementName.toStdString()))
    {
        QMessageBox::warning(this,"Add element","An element with name '"+elementName+"'' is already registered in the database.");
        return;
    }

    SX::Chemistry::sptrElement element=emgr->getElement(elementName.toStdString());

    try
    {
        for (auto p : _model->getIsotopes())
            element->addIsotope(p.first.toStdString(),p.second);
    }
    catch(const SX::Kernel::Error<SX::Chemistry::Element>& e)
    {
        QMessageBox::warning(this,"Add element",QString::fromStdString(e.what()));
        emgr->removeElement(element->getName());
        return;
    }

    emgr->saveRegistry();

}
