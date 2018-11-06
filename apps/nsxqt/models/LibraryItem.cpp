#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Logger.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>

#include "DataItem.h"
#include "DialogPredictPeaks.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "MetaTypes.h"
#include "PeakListItem.h"
#include "PeaksItem.h"
#include "ProgressView.h"
#include "UnitCellsItem.h"

LibraryItem::LibraryItem()
: TreeItem(),
  _library(nullptr)
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
        nsx::error() << "A library must be set for peak prediction";
        return;
    }

    nsx::debug() << "Incorporating missing peaks into current data set...";

    auto experiment_item = dynamic_cast<ExperimentItem*>(parent());

    auto data_item = experiment_item->dataItem();
    nsx::DataList numors = data_item->selectedData();

    auto unit_cells_item = experiment_item->unitCellsItem();

    DialogPredictPeaks *dialog = DialogPredictPeaks::create(experimentItem(), unit_cells_item->unitCells(), nullptr);

    dialog->show();

    dialog->raise();
}
