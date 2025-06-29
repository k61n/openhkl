//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ExperimentDialog.cpp
//! @brief     Implements class ExperimentDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ExperimentDialog.h"

#include "core/instrument/InstrumentParameters.h"
#include "base/utils/Path.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

ExperimentDialog::ExperimentDialog(QString exp_name)
{
    setWindowTitle("New experiment");
    setModal(true);
    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel("Name:"));
    experiment = new QLineEdit("");
    horizontalLayout->addWidget(experiment);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Instrument:"));
    instruments = new QComboBox();

    for (auto&& [key, value] : ohkl::Instrument::instrument_list)
        instruments->addItem(QString::fromStdString(value), QString::fromStdString(key));

    instruments->setInsertPolicy(QComboBox::InsertAlphabetically);
    horizontalLayout_2->addWidget(instruments);
    gridLayout->addLayout(horizontalLayout_2, 0, 1, 1, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 1, 1, 1, 1);

    experiment->setText(exp_name);
    experiment->selectAll();

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ExperimentDialog::experimentName()
{
    return experiment->text();
}

QString ExperimentDialog::instrumentName()
{
    return instruments->currentData().toString();
}
