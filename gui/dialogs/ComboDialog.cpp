//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ComboDialog.cpp
//! @brief     Implements class ComboDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ComboDialog.h"

#include "gui/MainWin.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>


ComboDialog::ComboDialog(QStringList items, QString description, QString title)
{
    setModal(true);
    setWindowTitle(title);

    QGridLayout* gridLayout = new QGridLayout(this);

    QLabel* label = new QLabel(description);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(label, 0, 0, 1, 1);
    _items_combo = new QComboBox();
    _items_combo->addItems(items);
    _items_combo->setInsertPolicy(QComboBox::InsertAlphabetically);
    gridLayout->addWidget(_items_combo, 0, 1, 1, 1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 2);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ComboDialog::itemName() const
{
    return _items_combo->currentText();
}

unsigned int ComboDialog::itemIndex() const
{
    return _items_combo->currentIndex();
}
