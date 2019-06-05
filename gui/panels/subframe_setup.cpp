//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_setup.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subframe_setup.h"

SubframeSetup::SubframeSetup() : QcrTabWidget {"property tabs"}
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270, 320);

    addTab((instrument = new TabInstrument), "Instrument"); // 0
    addTab((data = new TabData), "Data"); // 1
    addTab((unitcells = new TabUnitcells), "Unit cells"); // 2
    addTab((peaks = new TabPeaks), "Peaks"); // 3
    addTab((library = new TabPeaklibrary), "Peaklibrary"); // 4

    for (int i = 0; i < 4; i++)
        setTabEnabled(i, true);

    show();
}

void SubframeSetup::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}

void SubframeSetup::dataChanged()
{
    data->remake();
}

void SubframeSetup::experimentChanged()
{
    instrument->remake();
    unitcells->remake();
}

void SubframeSetup::peaksChanged()
{
    peaks->remake();
    library->remake();
}
