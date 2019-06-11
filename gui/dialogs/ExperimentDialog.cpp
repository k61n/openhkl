//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ExperimentDialog.cpp
//! @brief     Implements class ExperimentDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/dialogs/ExperimentDialog.h"
#include "core/instrument/HardwareParameters.h"
#include "base/utils/Path.h"
#include "gui/MainWin.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>

namespace {

QStringList allInstrumentNames()
{
    std::set<std::string> resources_name = nsx::getResourcesName("instruments");

    QDir diffractometersDirectory(QString::fromStdString(nsx::applicationDataPath()));
    diffractometersDirectory.cd("instruments");

    QStringList user_diffractometer_files =
        diffractometersDirectory.entryList({"*.yml"}, QDir::Files, QDir::Name);
    for (QString& diffractometer : user_diffractometer_files)
        resources_name.insert(QFileInfo(diffractometer).baseName().toStdString());

    QStringList list;
    for (auto res : resources_name)
        list.append(QString::fromStdString(res));

    return list;
}

} // namespace

ExperimentDialog::ExperimentDialog() : QcrModalDialog {"experimentDialog", gGui, "new Experiment"}
{
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);
    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel("Name:"));
    experiment = new QcrLineEdit("adhoc_experiment", "");
    experiment->setMaximumSize(200, 30);
    horizontalLayout->addWidget(experiment);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Instrument:"));
    instruments = new QcrComboBox("adhoc_instruments", new QcrCell<int>(0), allInstrumentNames());
    instruments->setInsertPolicy(QComboBox::InsertAlphabetically);
    horizontalLayout_2->addWidget(instruments);
    gridLayout->addLayout(horizontalLayout_2, 0, 1, 1, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 1, 1, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ExperimentDialog::experimentName()
{
    return experiment->getValue();
}

QString ExperimentDialog::instrumentName()
{
    return instruments->currentText();
}
