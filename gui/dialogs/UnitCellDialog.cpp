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

UnitCellDialog::UnitCellDialog(std::vector<std::string> collectionNames) : QDialog()
{
    QStringList list;
    for (auto name : collectionNames)
        list.push_back(QString::fromStdString(name));

    setModal(true);
    resize(500, 130);
    setMinimumSize(500, 130);
    setMaximumSize(500, 130);
    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel("Unit cell name:"));
    cellName = new QLineEdit("");
    cellName->setMaximumSize(200, 30);
    horizontalLayout->addWidget(cellName);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("Peak collection:"));
    peakCollections = new QComboBox();
    peakCollections->addItems(list);
    peakCollections->setInsertPolicy(QComboBox::InsertAlphabetically);
    horizontalLayout_2->addWidget(peakCollections);
    gridLayout->addLayout(horizontalLayout_2, 0, 1, 1, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 1, 1, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString UnitCellDialog::unitCellName()
{
    return cellName->text();
}

QString UnitCellDialog::peakCollectionName()
{
    return peakCollections->currentText();
}
