//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

ClonePeakDialog::ClonePeakDialog(QStringList collectionNames, const QString& suggestion)
{
    setModal(true);

    QGridLayout* gridLayout = new QGridLayout(this);

    QLabel* label = new QLabel("Collection to clone:");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _peak_collections = new QComboBox();
    _peak_collections->addItems(collectionNames);
    _peak_collections->setInsertPolicy(QComboBox::InsertAlphabetically);
    gridLayout->addWidget(label, 0, 0, 1, 1);
    gridLayout->addWidget(_peak_collections, 0, 1, 1, 1);

    label = new QLabel("New collection name:");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _new_collection_name = new QLineEdit();
    _new_collection_name->setText(suggestion);
    _new_collection_name->selectAll();
    gridLayout->addWidget(label, 1, 0, 1, 1);
    gridLayout->addWidget(_new_collection_name, 1, 1, 1, 1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 2, 0, 1, 2);

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
