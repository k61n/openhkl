//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/NewCellDialog.cpp
//! @brief     Implements class NewCellDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/NewCellDialog.h"

#include "base/utils/Path.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

NewCellDialog::NewCellDialog(QStringList spaceGroups)
{
    setModal(true);
    resize(600, 130);
    setMinimumSize(600, 130);
    setMaximumSize(600, 130);

    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;

    horizontalLayout->addWidget(new QLabel("Unit cell name:"));
    _cell_name = new QLineEdit("");
    _cell_name->setMaximumSize(200, 30);
    horizontalLayout->addWidget(_cell_name);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Space group:"));
    _space_group = new QComboBox();
    _space_group->addItems(spaceGroups);
    _space_group->setInsertPolicy(QComboBox::InsertAlphabetically);
    horizontalLayout_2->addWidget(_space_group);
    gridLayout->addLayout(horizontalLayout_2, 0, 2, 1, 1);

    _a = new QDoubleSpinBox();
    _a->setValue(10.0);
    _a->setMaximum(1000.0);
    _a->setDecimals(2);

    _b = new QDoubleSpinBox();
    _b->setValue(10.0);
    _b->setMaximum(1000.0);
    _b->setDecimals(2);

    _c = new QDoubleSpinBox();
    _c->setValue(10.0);
    _c->setMaximum(1000.0);
    _c->setDecimals(2);

    _alpha = new QDoubleSpinBox();
    _alpha->setValue(90.0);
    _alpha->setMaximum(180.0);
    _alpha->setDecimals(2);

    _beta = new QDoubleSpinBox();
    _beta->setValue(90.0);
    _beta->setMaximum(180.0);
    _beta->setDecimals(2);

    _gamma = new QDoubleSpinBox();
    _gamma->setValue(90.0);
    _gamma->setMaximum(180.0);
    _gamma->setDecimals(2);

    QHBoxLayout* horizongalLayout_3 = new QHBoxLayout;
    horizongalLayout_3->addWidget(new QLabel("Cell parameters:"));
    horizongalLayout_3->addWidget(_a);
    horizongalLayout_3->addWidget(_b);
    horizongalLayout_3->addWidget(_c);
    horizongalLayout_3->addWidget(_alpha);
    horizongalLayout_3->addWidget(_beta);
    horizongalLayout_3->addWidget(_gamma);
    gridLayout->addLayout(horizongalLayout_3, 1, 0, 1, 3);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 2, 1, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString NewCellDialog::unitCellName()
{
    return _cell_name->text();
}

QString NewCellDialog::spaceGroup()
{
    return _space_group->currentText();
}

double NewCellDialog::a()
{
    return _a->value();
}

double NewCellDialog::b()
{
    return _b->value();
}

double NewCellDialog::c()
{
    return _c->value();
}

double NewCellDialog::alpha()
{
    return _alpha->value();
}

double NewCellDialog::beta()
{
    return _beta->value();
}

double NewCellDialog::gamma()
{
    return _gamma->value();
}
