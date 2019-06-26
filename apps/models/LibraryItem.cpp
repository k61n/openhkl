//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/LibraryItem.cpp
//! @brief     Implements class LibraryItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "base/utils/ProgressHandler.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/shape/ShapeLibrary.h"

#include "apps/dialogs/DialogPredictPeaks.h"
#include "apps/models/DataItem.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/InstrumentItem.h"
#include "apps/models/LibraryItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/PeakListItem.h"
#include "apps/models/PeaksItem.h"
#include "apps/models/UnitCellsItem.h"
#include "apps/views/ProgressView.h"
#include <QtGlobal>
#include <QDebug>

LibraryItem::LibraryItem() : TreeItem(), _library(nullptr)
{
    setText("Reference peak library");

    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);

    setDragEnabled(false);
    setDropEnabled(false);

    setEditable(false);

    setSelectable(false);
}

void LibraryItem::incorporateCalculatedPeaks()
{
    if (!_library) {
        qWarning() << "A library must be set for peak prediction";
        return;
    }

    qDebug() << "Incorporating missing peaks into current data set...";

    auto experiment_item = dynamic_cast<ExperimentItem*>(parent());

    auto data_item = experiment_item->dataItem();
    nsx::DataList numors = data_item->selectedData();

    auto unit_cells_item = experiment_item->unitCellsItem();

    DialogPredictPeaks* dialog =
        DialogPredictPeaks::create(experimentItem(), unit_cells_item->unitCells(), nullptr);

    dialog->show();

    dialog->raise();
}
