//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ListNameDialog.cpp
//! @brief     Implements class ListNameDialog
//!
//! @homepage  https://openhkl.org
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
    _name = new QLineEdit();
    line->addWidget(new QLabel("Peak collection name: "));
    line->addWidget(_name);
    whole->addLayout(line);
    QDialogButtonBox* buttonbox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    whole->addWidget(buttonbox);

    _name->setText(suggestion);
    _name->selectAll();

    QObject::connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ListNameDialog::listName()
{
    if (_name->text() == "")
        return "unnamed peak collection";
    return _name->text();
}
