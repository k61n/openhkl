#include <QComboBox>

#include <nsxlib/instrument/DiffractometerStore.h>

#include "DialogExperiment.h"
#include "ui_ExperimentDialog.h"

DialogExperiment::DialogExperiment(QWidget *parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    // Add the available instruments to the combo box
    nsx::DiffractometerStore* ds = nsx::DiffractometerStore::Instance();

    auto diffractometers=ds->getDiffractometersList();
    for (const auto& d : diffractometers)
        ui->instrument->addItem(QString::fromStdString(d));
}

DialogExperiment::~DialogExperiment()
{
    delete ui;
}

QString DialogExperiment::getExperimentName() const
{
    return ui->experiment->toPlainText();
}

QString DialogExperiment::getInstrumentName() const
{
    return ui->instrument->currentText();
}
