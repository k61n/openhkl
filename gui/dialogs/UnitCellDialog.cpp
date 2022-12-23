//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/UnitCellDialog.cpp
//! @brief     Implements class UnitCellDialog
//!
//! @homepage  https://openhkl.org
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
    QString suggestion, QStringList spaceGroups, QStringList collectionNames)
{
    setModal(true);

    QGridLayout* gridLayout = new QGridLayout(this);

    int row = 0;
    QLabel* label = new QLabel("Unit cell name:");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _cell_name = new QLineEdit("");
    _cell_name->setFixedWidth(200);
    gridLayout->addWidget(label, row, 0, 1, 1);
    gridLayout->addWidget(_cell_name, row++, 1, 1, 2);

    label = new QLabel("Space group:");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _space_group = new QComboBox();
    _space_group->addItems(spaceGroups);
    _space_group->setInsertPolicy(QComboBox::InsertAlphabetically);
    _space_group->setFixedWidth(200);
    gridLayout->addWidget(label, row, 0, 1, 1);
    gridLayout->addWidget(_space_group, row++, 1, 1, 2);

    if (!collectionNames.empty()) {
        label = new QLabel("Peak collection:");
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        _peak_collections = new QComboBox();
        _peak_collections->addItems(collectionNames);
        _peak_collections->setInsertPolicy(QComboBox::InsertAlphabetically);
        _peak_collections->setFixedWidth(200);
        gridLayout->addWidget(label, row, 0, 1, 1);
        gridLayout->addWidget(_peak_collections, row++, 1, 1, 2);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, row, 0, 1, 3, Qt::AlignCenter);

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
