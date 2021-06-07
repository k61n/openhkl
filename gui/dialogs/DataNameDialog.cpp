//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/DataNameDialog.cpp
//! @brief     Implements class DataNameDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/DataNameDialog.h"

#include "base/utils/Path.h"
#include "core/instrument/HardwareParameters.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QString>

DataNameDialog::DataNameDialog(const std::string dataname)
{
    setModal(true);
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);
    QGridLayout* gridLayout = new QGridLayout(this);

    QLabel* const dataname_label {new QLabel(QString::fromStdString("Data name:"))};
    dataname_ledit = new QLineEdit(QString::fromStdString(dataname));

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    gridLayout->addWidget(dataname_label, 0, 0, 1, 1);
    gridLayout->addWidget(dataname_ledit, 0, 1, 1, 1);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString DataNameDialog::dataName()
{
    return dataname_ledit->text();
}
