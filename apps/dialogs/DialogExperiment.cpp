//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogExperiment.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include "core/utils/Path.h"
#include "Resources.h"

#include "apps/dialogs/DialogExperiment.h"
#include "ui_DialogExperiment.h"

DialogExperiment::DialogExperiment(QWidget* parent) : QDialog(parent), ui(new Ui::DialogExperiment)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->instrument->setInsertPolicy(QComboBox::InsertAlphabetically);

    auto resources_name = nsx::getResourcesName("instruments");

    QDir diffractometersDirectory(QString::fromStdString(nsx::applicationDataPath()));
    diffractometersDirectory.cd("instruments");

    QStringList user_diffractometer_files =
        diffractometersDirectory.entryList({"*.yml"}, QDir::Files, QDir::Name);
    for (auto&& diffractometer : user_diffractometer_files) {
        resources_name.insert(QFileInfo(diffractometer).baseName().toStdString());
    }

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
