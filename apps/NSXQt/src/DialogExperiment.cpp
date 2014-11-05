#include "DialogExperiment.h"
#include "ui_DialogExperiment.h"
#include <QString>
#include "DiffractometerFactory.h"
#include <QComboBox>

using namespace SX::Instrument;

DialogExperiment::DialogExperiment(QWidget *parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    // Add the available instruments to the combo box
    DiffractometerFactory* f = DiffractometerFactory::Instance();
    for (auto instr : f->list())
        ui->instrument->addItem(QString::fromStdString(instr));
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
