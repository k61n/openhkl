//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ConfirmOverwriteDialog.cpp
//! @brief     Implements class ConfirmOverwriteDialog
//!
//! @homepage  ###HOMEPAGE###
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
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);

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
