//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ClonePeakDialog.cpp
//! @brief     Implements class ClonePeakDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ClonePeakDialog.h"

#include "base/utils/Path.h"
#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

ClonePeakDialog::ClonePeakDialog(QStringList collectionNames)
{
    setModal(true);
    resize(600, 130);
    setMinimumSize(600, 130);
    setMaximumSize(600, 130);

    QGridLayout* gridLayout = new QGridLayout(this);

    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel("Collection to clone:"));
    _peak_collections = new QComboBox();
    _peak_collections->addItems(collectionNames);
    _peak_collections->setInsertPolicy(QComboBox::InsertAlphabetically);
    horizontalLayout->addWidget(_peak_collections);
    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout;
    horizontalLayout_2->addWidget(new QLabel("New collection name:"));
    _new_collection_name = new QLineEdit("");
    _new_collection_name->setMaximumSize(200, 30);
    horizontalLayout_2->addWidget(_new_collection_name);
    gridLayout->addLayout(horizontalLayout_2, 0, 1, 1, 1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 1, 1, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ClonePeakDialog::originalCollectionName()
{
    return _peak_collections->currentText();
}

QString ClonePeakDialog::clonedCollectionName()
{
    return _new_collection_name->text();
}
