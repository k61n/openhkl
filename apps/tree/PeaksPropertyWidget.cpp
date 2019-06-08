//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/PeaksPropertyWidget.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <memory>

#include <QSortFilterProxyModel>

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include "apps/models/ExperimentItem.h"
#include "apps/models/InstrumentItem.h"
#include "apps/views/PeakTableView.h"
#include "apps/models/PeaksItem.h"
#include "apps/tree/PeaksPropertyWidget.h"
#include "apps/models/SampleItem.h"
#include "apps/models/UnitCellItem.h"

#include "ui_PeaksPropertyWidget.h"

PeaksPropertyWidget::PeaksPropertyWidget(PeaksItem* peaks_item, QWidget* parent)
    : QWidget(parent), _peaks_item(peaks_item), ui(new Ui::PeaksPropertyWidget)
{
    ui->setupUi(this);

    _peaks_model = new CollectedPeaksModel(
        _peaks_item->model(), _peaks_item->experiment(), _peaks_item->selectedPeaks());

    ui->tableView->setModel(_peaks_model);
}

PeaksPropertyWidget::~PeaksPropertyWidget()
{
    delete ui;
}

CollectedPeaksModel* PeaksPropertyWidget::model()
{
    return _peaks_model;
}
