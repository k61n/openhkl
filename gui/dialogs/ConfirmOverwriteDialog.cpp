//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ConfirmOverwriteDialog.cpp
//! @brief     Implements class ConfirmOverwriteDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ConfirmOverwriteDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>

ConfirmOverwriteDialog::ConfirmOverwriteDialog(const QString& name)
{
    setModal(true);
    setWindowTitle("Confirm");

    const std::string msg{"The name '" + name.toStdString() + "' will be overwritten."};
    QLabel* const dataname_label{new QLabel(QString::fromStdString(msg))};

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::No | QDialogButtonBox::Yes);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(dataname_label);
    vLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    this->setLayout(vLayout);
}
