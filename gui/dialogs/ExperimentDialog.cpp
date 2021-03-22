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

#include "base/utils/Path.h"
#include "core/instrument/HardwareParameters.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

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
    for (std::string res : resources_name)
        list.push_back(QString::fromStdString(res));

    return list;
}

} // namespace

ExperimentDialog::ExperimentDialog()
{
    setModal(true);
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);
    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel("Name:"));
    experiment = new QLineEdit("");
    experiment->setMaximumSize(200, 30);
    horizontalLayout->addWidget(experiment);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Instrument:"));
    instruments = new QComboBox();
    instruments->addItems(allInstrumentNames());
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
    return experiment->text();
}

QString ExperimentDialog::instrumentName()
{
    return instruments->currentText();
}
