#include "DataItem.h"
#include "DialogCalculatedPeaks.h"
#include "ExperimentItem.h"
#include "LibraryItem.h"
#include "PeakListItem.h"
#include "PeaksItem.h"
#include "ProgressView.h"

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Logger.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>

LibraryItem::LibraryItem(): TreeItem(), _library(new nsx::ShapeLibrary(false))
{
    setText("Reference peak library");
    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
}

void LibraryItem::incorporateCalculatedPeaks()
{
    nsx::debug() << "Incorporating missing peaks into current data set...";

    auto& expt_item = dynamic_cast<ExperimentItem&>(*parent());
    auto& data_item = expt_item.dataItem();

    std::set<nsx::sptrUnitCell> cells;

    nsx::DataList numors = data_item.selectedData();

    for (auto numor: numors) {
        auto sample = numor->diffractometer()->getSample();

        for (auto uc: sample->unitCells()) {
            cells.insert(uc);
        }
    }

    DialogCalculatedPeaks dialog(cells);

    if (!dialog.exec()) {
        return;
    }

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    int current_numor = 0;
    int observed_peaks = 0;

    // TODO: get the crystal from the dialog!!
    auto cell = dialog.cell();

    nsx::PeakList predicted_peaks;

    for(auto numor: numors) {
        nsx::debug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

        auto predictor = nsx::PeakPredictor(cell, dialog.dMin(), dialog.dMax(), _library);
        auto predicted = predictor.predict(numor, dialog.radius(), dialog.nframes());

        for (auto peak: predicted) {
            predicted_peaks.push_back(peak);
        }

        nsx::debug() << "Added " << predicted.size() << " predicted peaks.";
    }
    auto peaks_item = expt_item.peaks().createPeaksItem("Predicted peaks");
    std::swap(peaks_item->peaks(), predicted_peaks);
    //updatePeaks();
}
