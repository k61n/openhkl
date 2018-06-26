#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include <nsxlib/Path.h>
#include <nsxlib/Resources.h>

#include "DialogExperiment.h"
#include "ui_DialogExperiment.h"

DialogExperiment::DialogExperiment(QWidget *parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    auto resources_name = nsx::getResourcesName("instruments");

    for (auto res_name : resources_name) {
        ui->instrument->addItem(QString::fromStdString(res_name));
    }
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
