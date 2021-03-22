//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/properties/PeakProperties.cpp
//! @brief     Implements classes ListTab, PeakProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/PeakProperties.h"

#include "gui/MainWin.h"
#include "gui/dialogs/ClonePeakDialog.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/SideBar.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

PeakProperties::PeakProperties()
{
    setSizePolicies();

    _peak_list_combo = new QComboBox();

    _peak_table = new PeakTableView;
    _add = new QPushButton();
    _filter = new QPushButton();
    _remove = new QPushButton();

    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* top_layout = new QHBoxLayout;

    top_layout->addWidget(_peak_list_combo);
    top_layout->addWidget(_add);
    top_layout->addWidget(_filter);
    top_layout->addWidget(_remove);

    layout->addLayout(top_layout);
    layout->addWidget(_peak_table);

    _add->setIcon(QIcon(":/images/Add_item.svg"));
    _filter->setIcon(QIcon(":/images/filterIcon.svg"));
    _remove->setIcon(QIcon(":/images/Delete_item.svg"));

    _peak_list_combo->setSizePolicy(*_size_policy_widgets);

    _type = new QLabel;
    _file_name = new QLabel;
    _kernel_name = new QLabel;
    _parent_name = new QLabel;
    _peak_num = new QLabel;
    _valid = new QLabel;
    _non_valid = new QLabel;

    QFormLayout* left_side = new QFormLayout;
    left_side->addRow("Type:", _type);
    left_side->addRow("Peaks:", _peak_num);
    left_side->addRow("Valid:", _valid);
    left_side->addRow("Not valid:", _non_valid);

    QHBoxLayout* meta_box = new QHBoxLayout;
    meta_box->addLayout(left_side);

    layout->addLayout(meta_box);

    connect(
        _peak_list_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &PeakProperties::selectedPeaksChanged);

    connect(_add, &QPushButton::clicked, this, &PeakProperties::addMenuRequested);

    connect(_filter, &QPushButton::clicked, this, &PeakProperties::jumpToFilter);

    connect(_remove, &QPushButton::clicked, this, &PeakProperties::deleteCollection);
}

void PeakProperties::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void PeakProperties::refreshInput()
{
    _peak_list = gSession->currentProject()->getPeakListNames();
    _peak_list_combo->blockSignals(true);
    _peak_list_combo->clear();
    _peak_list_combo->addItems(_peak_list);
    _peak_list_combo->blockSignals(false);

    if (!gSession->currentProject()->getPeakListNames().empty())
        selectedPeaksChanged();
}

void PeakProperties::selectedPeaksChanged()
{
    PeakCollectionModel* model =
        gSession->currentProject()->peakModelAt(_peak_list_combo->currentIndex());
    _peak_table->setModel(model);

    if (!model) {
        _type->setText("");
        _peak_num->setText("");
        _valid->setText("");
        _non_valid->setText("");
        return;
    }

    nsx::listtype listType = model->root()->peakCollection()->type();
    switch (listType) {
        case nsx::listtype::FILTERED: {
            _type->setText("Filtered");
            break;
        }
        case nsx::listtype::FOUND: {
            _type->setText("Found");
            break;
        }
        case nsx::listtype::PREDICTED: {
            _type->setText("Predicted");
            break;
        }
        case nsx::listtype::INDEXING: {
            _type->setText("Indexing");
            break;
        }
    }

    _peak_num->setText(QString::number(model->root()->peakCollection()->numberOfPeaks()));
    _valid->setText(QString::number(model->root()->peakCollection()->numberOfValid()));
    _non_valid->setText(QString::number(model->root()->peakCollection()->numberOfInvalid()));
}

void PeakProperties::currentProjectChanged()
{
    if (gSession->currentProjectNum() < 0)
        return;

    selectedPeaksChanged();
}

void PeakProperties::addMenuRequested()
{
    QMenu* menu = new QMenu(_add);

    QAction* add_from_finder = menu->addAction("Add from peak finder");
    QAction* add_from_predictor = menu->addAction("Add from peak predictor");
    QAction* clone_collection = menu->addAction("Clone peak collection");

    connect(add_from_finder, &QAction::triggered, this, &PeakProperties::jumpToFinder);

    connect(add_from_predictor, &QAction::triggered, this, &PeakProperties::jumpToPredictor);

    connect(clone_collection, &QAction::triggered, this, &PeakProperties::clonePeakCollection);

    menu->popup(mapToGlobal(_add->geometry().bottomLeft()));
}

void PeakProperties::jumpToFinder()
{
    gGui->sideBar()->manualSelect(2);
}

void PeakProperties::jumpToPredictor()
{
    gGui->sideBar()->manualSelect(5);
}

void PeakProperties::jumpToFilter()
{
    gGui->sideBar()->manualSelect(3);
}

void PeakProperties::clonePeakCollection()
{
    if (!_peak_list.empty()) {
        std::unique_ptr<ClonePeakDialog> dlg(new ClonePeakDialog(_peak_list));
        dlg->exec();
        if (!dlg->clonedCollectionName().isEmpty()) {
            QString original = dlg->originalCollectionName();
            QString cloned = dlg->clonedCollectionName();
            gSession->currentProject()->clonePeakCollection(original, cloned);
        }
    }
    refreshInput();
}

void PeakProperties::deleteCollection()
{
    _peak_table->setModel(nullptr);

    gSession->currentProject()->removePeakModel(_peak_list_combo->currentText());
    refreshInput();
}
