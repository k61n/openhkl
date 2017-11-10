#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include <nsxlib/Path.h>

#include "DialogExperiment.h"
#include "ui_ExperimentDialog.h"

DialogExperiment::DialogExperiment(QWidget *parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    QDir diffractometersDirectory(QString::fromStdString(nsx::applicationDataPath()));
    diffractometersDirectory.cd("instruments");

    QStringList diffractometerFiles = diffractometersDirectory.entryList({"*.yml"}, QDir::Files, QDir::Name);

    for (auto&& diffractometer : diffractometerFiles)
        ui->instrument->addItem(QFileInfo(diffractometer).baseName());
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
