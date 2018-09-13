#include <fstream>

#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include <nsxlib/CC.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/RFactor.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>

#include "DataItem.h"
#include "DialogStatistics.h"
#include "DetectorItem.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "PeaksItem.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"
#include "UnitCellsItem.h"

ExperimentItem::ExperimentItem(nsx::sptrExperiment experiment): TreeItem(), _experiment(experiment)
{
    setText(QString::fromStdString(experiment->name()));

    setForeground(QBrush(QColor("blue")));

    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);

    setDragEnabled(false);
    setDropEnabled(false);

    setEditable(true);

    setSelectable(true);

    setCheckable(false);

    // Create an instrument item and add it to the experiment item
    auto diff = experiment->diffractometer();
    _instr = new InstrumentItem(diff->name().c_str(), diff->source()->name().c_str());
    appendRow(_instr);

    // Create a data item and add it to the experiment item
    _data = new DataItem();
    appendRow(_data);

    // Create the reference peak library
    _unitCells = new UnitCellsItem();
    appendRow(_unitCells);

    // Create a peaks item and add it to the experiment item
    _peaks = new PeaksItem();
    appendRow(_peaks);

    // Create the reference peak library
    _library = new LibraryItem();
    appendRow(_library);
}

ExperimentItem::~ExperimentItem()
{
}

InstrumentItem* ExperimentItem::instrumentItem()
{
    return _instr;
}

PeaksItem* ExperimentItem::peaksItem()
{
    return _peaks;
}

DataItem* ExperimentItem::dataItem()
{
    return _data;
}

LibraryItem* ExperimentItem::libraryItem()
{
    return _library;
}

UnitCellsItem* ExperimentItem::unitCellsItem()
{
    return _unitCells;
}

void ExperimentItem::writeLogFiles()
{
    auto&& peaks = _peaks->selectedPeaks();

    nsx::PeakFilter peak_filter;
    nsx::PeakList filtered_peaks;
    filtered_peaks = peak_filter.enabled(peaks,true);
    filtered_peaks = peak_filter.hasUnitCell(filtered_peaks);

    if (filtered_peaks.empty()) {
        nsx::error() << "No valid peaks in the table";
        return;
    }

    auto cell = filtered_peaks[0]->unitCell();

    filtered_peaks = peak_filter.unitCell(filtered_peaks,cell);
    filtered_peaks = peak_filter.indexed(filtered_peaks,cell,cell->indexingTolerance());

    DialogStatistics dlg(filtered_peaks,cell->spaceGroup());

    if (!dlg.exec()) {
        return;
    }
}
