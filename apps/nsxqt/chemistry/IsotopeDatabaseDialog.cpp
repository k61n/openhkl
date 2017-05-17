#include <nsxlib/chemistry/IsotopeDatabaseManager.h>

#include "IsotopeDatabaseDialog.h"

#include "ui_IsotopeDatabaseDialog.h"

IsotopeDatabaseDialog::IsotopeDatabaseDialog(QWidget *parent) : QDialog(parent), ui(new Ui::IsotopeDatabaseDialog)
{
    ui->setupUi(this);

    // Make sure that the user can not edit the content of the table
    ui->isotopeDatabaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    nsx::IsotopeDatabaseManager* imgr=nsx::IsotopeDatabaseManager::Instance();

    resize(1000,500);
    ui->isotopeDatabaseView->show();
}

IsotopeDatabaseDialog::~IsotopeDatabaseDialog()
{
    delete ui;
}
