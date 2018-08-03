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
#include "SampleItem.h"

LibraryItem::LibraryItem()
: TreeItem(),
  _library(nullptr)
{
    setText("Reference peak library");
    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);
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

    auto sample_item = experiment_item->instrumentItem()->sampleItem();

    DialogPredictPeaks dialog(sample_item->unitCells());

    if (!dialog.exec()) {
        return;
    }

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    int current_numor = 0;

    auto cell = dialog.cell();
    auto&& d_min = dialog.dMin();
    auto&& d_max = dialog.dMax();
    auto&& radius = dialog.radius();
    auto&& n_frames = dialog.nFrames();
    auto&& min_neighbors = dialog.minNeighbors();
    nsx::PeakInterpolation interpolation = static_cast<nsx::PeakInterpolation>(dialog.interpolation());

    nsx::PeakList predicted_peaks;

    for(auto numor: numors) {
        nsx::debug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

        auto&& predicted = nsx::predictPeaks(*_library, numor, cell, d_min, d_max, radius, n_frames, min_neighbors, interpolation);

        for (auto peak: predicted) {
            predicted_peaks.push_back(peak);
        }

        nsx::debug() << "Added " << predicted.size() << " predicted peaks.";
    }

    auto peaks_item = experimentItem()->peaksItem();
    auto item = new PeakListItem(predicted_peaks);
    item->setText("Predicted peaks");
    peaks_item->appendRow(item);
}
