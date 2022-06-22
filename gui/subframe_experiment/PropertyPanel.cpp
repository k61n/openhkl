//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/PropertyPanel.cpp
//! @brief     Implements class PropertyPanel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/PropertyPanel.h"

#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_experiment/properties/InstrumentProperty.h"
#include "gui/subframe_experiment/properties/NumorProperty.h"
#include "gui/subframe_experiment/properties/PeakProperties.h"
#include "gui/subframe_experiment/properties/UnitCellProperty.h"

namespace {
enum class tab { INSTRUMENT, DATA, UNITCELLS, PEAKS };

} // namespace

PropertyPanel::PropertyPanel()
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270, 320);

    addTab((_instrument = new InstrumentProperty), "Instrument"); // 0
    addTab((_data = new NumorProperty), "Data"); // 1
    addTab((_unitcells = new UnitCellProperty), "Unit cells"); // 2
    addTab((_peaks = new PeakProperties), "Peaks"); // 3
}

void PropertyPanel::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void PropertyPanel::dataChanged() const
{
    // bool enabled = gSession->currentProjectNum() >= 0;
    // if (enabled)
    //     enabled = !gSession->currentProject()->getDataNames().empty();
    // setTabEnabled((int)tab::DATA, enabled);
    if (!gSession->currentProject()->getDataNames().empty())
        _data->refreshInput();
}

void PropertyPanel::experimentChanged() const
{
    //_peaks->currentProjectChanged();
   // _instrument->refreshInput();
}

void PropertyPanel::peaksChanged() const
{
    bool enabled = gSession->currentProjectNum() >= 0;
    if (enabled)
        _peaks->refreshInput();
    _peaks->selectedPeaksChanged();
}

void PropertyPanel::unitCellChanged() const
{
    bool enabled = gSession->currentProjectNum() >= 0;
    if (enabled)
        _unitcells->refreshInput();
}
