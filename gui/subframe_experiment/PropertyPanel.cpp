//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/PropertyPanel.cpp
//! @brief     Implements class PropertyPanel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/PropertyPanel.h"

#include "gui/models/Session.h"

namespace {
enum class tab { INSTRUMENT, DATA, UNITCELLS, PEAKS };

} // namespace

PropertyPanel::PropertyPanel() : QTabWidget()
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270, 320);

    addTab((instrument = new InstrumentProperty), "Instrument"); // 0
    addTab((data = new NumorProperty), "Data"); // 1
    addTab((unitcells = new UnitCellProperty), "Unit cells"); // 2
    addTab((peaks = new PeakProperties), "Peaks"); // 3
}

void PropertyPanel::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void PropertyPanel::dataChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    if (enabled)
        enabled = !gSession->selectedExperiment()->getDataNames().empty();
    setTabEnabled((int)tab::DATA, enabled);
    if (enabled)
        data->refreshInput();
}

void PropertyPanel::experimentChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    setTabEnabled((int)tab::INSTRUMENT, enabled);
    setTabEnabled((int)tab::UNITCELLS, enabled);
    peaks->selectedExperimentChanged();
}

void PropertyPanel::peaksChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    if (enabled)
        enabled = gSession->selectedExperiment()->getPeakListNames().size() > 0;
    setTabEnabled((int)tab::PEAKS, enabled);
    if (enabled)
        peaks->refreshInput();
        peaks->selectedPeaksChanged();
}

void PropertyPanel::unitCellChanged()
{
    bool enabled = gSession->selectedExperimentNum() >= 0;
    if (enabled)
        enabled = gSession->selectedExperiment()->getUnitCellNames().size() > 0;
    setTabEnabled((int)tab::UNITCELLS, enabled);
    if (enabled)
        unitcells->refreshInput();
}
