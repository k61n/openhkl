#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include <nsxlib/Path.h>
#include <nsxlib/ResourcesMap.h>

#include "DialogExperiment.h"
#include "ui_ExperimentDialog.h"

DialogExperiment::DialogExperiment(QWidget *parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    for (auto resource : nsx::getResources()) {
        auto res = resource.first;
        if (res.first.compare("instruments")) {
            ui->instrument->addItem(QString::fromStdString(res.second));
        }
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
