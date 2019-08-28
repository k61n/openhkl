//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeSetup.cpp
//! @brief     Implements class SubframeSetup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeSetup.h"

#include "gui/models/Session.h"

namespace {
enum class tab { INSTRUMENT, DATA, UNITCELLS, PEAKS };

} // namespace

SubframeSetup::SubframeSetup() : QcrTabWidget {"property tabs"}
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270, 320);

    addTab((instrument = new TabInstrument), "Instrument"); // 0
    addTab((data = new NumorProperty), "Data"); // 1
    addTab((unitcells = new UnitCellProperty), "Unit cells"); // 2
    addTab((peaks = new TabPeaks), "Peaks"); // 3

    show();
}

void SubframeSetup::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void SubframeSetup::dataChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    if (enabled)
        enabled = !gSession->selectedExperiment()->getDataNames().empty();
    setTabEnabled((int)tab::DATA, enabled);
    if (enabled)
        data->remake();
}

void SubframeSetup::experimentChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    setTabEnabled((int)tab::INSTRUMENT, enabled);
    setTabEnabled((int)tab::UNITCELLS, enabled);
    instrument->remake();
    unitcells->remake();
    peaks->selectedExperimentChanged();
}

void SubframeSetup::peaksChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    if (enabled)
        enabled = gSession->selectedExperiment()->getPeakListNames().size() > 0;
    setTabEnabled((int)tab::PEAKS, enabled);
    if (enabled)
        peaks->selectedPeaksChanged();
}
