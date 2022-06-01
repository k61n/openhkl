//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "gui/dialogs/ConfirmOverwriteDialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QString>

DataNameDialog::DataNameDialog(const QString& dataname, const QStringList* const datanames_cur)
    : datanames{datanames_cur}
{
    setModal(true);
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);

    QGridLayout* gridLayout = new QGridLayout(this);

    QLabel* const dataname_label{new QLabel(QString::fromStdString("Data name:"))};
    _dataname_ledit = new QLineEdit(dataname);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    gridLayout->addWidget(dataname_label, 0, 0, 1, 1);
    gridLayout->addWidget(_dataname_ledit, 0, 1, 1, 1);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &DataNameDialog::verify);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString DataNameDialog::dataName()
{
    return _dataname_ledit->text();
}

void DataNameDialog::verify()
{
    // confirm overwrite if the name already exists
    const QString dname = dataName();
    const bool name_exists = datanames->contains(dname);
    bool dialog_accepted = true;

    if (name_exists) {
        ConfirmOverwriteDialog overwrite_dialog(dname);
        overwrite_dialog.exec();
        if (overwrite_dialog.result() == QDialog::Rejected)
            dialog_accepted = false;
    }

    if (dialog_accepted)
        this->accept();
}
