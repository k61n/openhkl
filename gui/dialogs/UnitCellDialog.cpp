//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/UnitCellDialog.cpp
//! @brief     Implements class UnitCellDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/UnitCellDialog.h"

#include "base/utils/Path.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

UnitCellDialog::UnitCellDialog(
    QString suggestion, QStringList collectionNames, QStringList spaceGroups)
{
    setModal(true);
    resize(400, 230);
    setMinimumSize(400, 230);
    setMaximumSize(400, 230);

    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;

    horizontalLayout->addWidget(new QLabel("Unit cell name:"));
    _cell_name = new QLineEdit("");
    _cell_name->setFixedWidth(200);
    //_cell_name->setMaximumSize(200, 30);
    horizontalLayout->addWidget(_cell_name);
    gridLayout->addLayout(horizontalLayout, 0, 0, 3, 2);

    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Peak collection:"));
    _peak_collections = new QComboBox();
    _peak_collections->addItems(collectionNames);
    _peak_collections->setInsertPolicy(QComboBox::InsertAlphabetically);
    _peak_collections->setFixedWidth(200);
    //_peak_collections->setMaximumSize(200, 30);
    horizontalLayout_2->addWidget(_peak_collections);
    gridLayout->addLayout(horizontalLayout_2, 1, 0, 3, 2);

    QHBoxLayout* horizontalLayout_3 = new QHBoxLayout;
    horizontalLayout_3->addWidget(new QLabel("Space group:"));
    _space_group = new QComboBox();
    _space_group->addItems(spaceGroups);
    _space_group->setInsertPolicy(QComboBox::InsertAlphabetically);
    _space_group->setFixedWidth(200);
    //_space_group->setMaximumSize(200, 30);
    horizontalLayout_3->addWidget(_space_group);
    gridLayout->addLayout(horizontalLayout_3, 2, 0, 3, 2);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 3, 0, 3, 2);

    _cell_name->setText(suggestion);
    _cell_name->selectAll();

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString UnitCellDialog::unitCellName()
{
    return _cell_name->text();
}

QString UnitCellDialog::peakCollectionName()
{
    return _peak_collections->currentText();
}

QString UnitCellDialog::spaceGroup()
{
    return _space_group->currentText();
}
