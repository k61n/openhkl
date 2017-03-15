#include <nsxlib/chemistry/ChemicalDatabaseManager.h>
#include <nsxlib/chemistry/Element.h>
#include <nsxlib/chemistry/ElementManager.h>
#include <nsxlib/chemistry/Isotope.h>
#include <nsxlib/kernel/Error.h>

#include "ui_ElementManagerDialog.h"
#include "ElementManagerDialog.h"
#include "DragElementModel.h"

#include <iostream>
#include <string>
#include <vector>

#include <QListIterator>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QShortcut>
#include <QSortFilterProxyModel>

using SX::Chemistry::ChemicalDatabaseManager;
using SX::Chemistry::Isotope;

ElementManagerDialog::ElementManagerDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::ElementManagerDialog)
{
    ui->setupUi(this);

    // Get the element manager
    _elementMgr = SX::Chemistry::ElementManager::Instance();

    // Fills the element list widget with the elements stored in the registry
    for (const auto& p : _elementMgr->getRegistry())
        ui->elementsList->addItem(QString::fromStdString(p.first));

    // Get the isotope manager
    ChemicalDatabaseManager<Isotope>* _isotopeMgr = ChemicalDatabaseManager<Isotope>::Instance();
    // Fills the element list widget with the isotopes stored in the registry
    for (const auto& isotope : _isotopeMgr->getDatabase())
        ui->isotopesList->addItem(QString::fromStdString(isotope.first));

    // Create and set the model underlying the new element table view
    _model = new DragElementModel();
    ui->selectedIsotopesView->setModel(_model);

    // Pressing Delete keyboard key on one element of the elements list widget will delete this element from the Elements registry
    QShortcut* deleteShortcut1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->elementsList,nullptr,nullptr,Qt::WidgetShortcut);
    connect(deleteShortcut1, SIGNAL(activated()), this, SLOT(deleteElement()));

    // Pressing Delete keyboard key on one isotope new element table view widget will delete this isotope
    // from the list of elements to be added
    QShortcut* deleteShortcut2 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->selectedIsotopesView,nullptr,nullptr,Qt::WidgetShortcut);
    connect(deleteShortcut2, SIGNAL(activated()), this, SLOT(removeIsotope()));

}

ElementManagerDialog::~ElementManagerDialog()
{
    delete ui;
}

void ElementManagerDialog::on_setButton_clicked()
{

    // Gets the element name
    QString elementName=ui->elementNameEntry->text().simplified();

    // Checks that it is not empty, otherwise return
    if (elementName.isEmpty())
    {
        QMessageBox::warning(this,"Add element","Empty element name.");
        return;
    }

    try
    {
        _model->buildElement(elementName);
    }
    catch (SX::Kernel::Error<SX::Chemistry::Element>& e)
    {
        QMessageBox::warning(this,"Add element",QString::fromStdString(e.what()));
        _elementMgr->removeElement(elementName.toStdString());
        return;
    }
    auto matches=ui->elementsList->findItems(elementName,Qt::MatchExactly);
    if (matches.isEmpty())
        ui->elementsList->addItem(elementName);
}

void ElementManagerDialog::on_saveButton_clicked()
{
    _elementMgr->saveRegistry();
}

void ElementManagerDialog::on_okButton_clicked()
{
    destroy();
}

void ElementManagerDialog::deleteElement()
{
    QString elementName=ui->elementsList->currentItem()->text();
    _elementMgr->removeElement(elementName.toStdString());
    delete ui->elementsList->currentItem();
}

void ElementManagerDialog::removeIsotope()
{

    QItemSelectionModel* selectionModel = ui->selectedIsotopesView->selectionModel();

    QModelIndexList indexes = selectionModel->selection().indexes();
    QListIterator<QModelIndex> it(indexes);
    it.toBack();
    while (it.hasPrevious())
        _model->removeRows(it.previous().row(), 1, QModelIndex());
}
