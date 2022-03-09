//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ListNameDialog.cpp
//! @brief     Implements class ListNameDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ListNameDialog.h"

#include "gui/MainWin.h"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

ListNameDialog::ListNameDialog(QString suggestion)
{
    setModal(true);
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);

    QVBoxLayout* whole = new QVBoxLayout(this);
    QHBoxLayout* line = new QHBoxLayout;
    name_ = new QLineEdit();
    line->addWidget(new QLabel("Peak collection name: "));
    line->addWidget(name_);
    whole->addLayout(line);
    QDialogButtonBox* buttonbox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    whole->addWidget(buttonbox);

    name_->setText(suggestion);

    QObject::connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ListNameDialog::listName()
{
    if (name_->text() == "")
        return "unnamed peak collection";
    return name_->text();
}
