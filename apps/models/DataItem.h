//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/DataItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>
#include <core/PeakFinder.h>

#include "TreeItem.h"

class MainWindow;
class NumorItem;

class DataItem : public TreeItem {
public:
    explicit DataItem();

    void importData();

    void importRawData();

    void importRawData(
        const std::vector<std::string>& filenames, double wavelength, double delta_chi,
        double delta_omega, double delta_phi, bool rowMajor, bool swapEndian, int bpp);

    void findPeaks();

    void openInstrumentStatesDialog();

    nsx::DataList allData();

    nsx::DataList selectedData();

    void convertToHDF5();

    void removeSelectedData();
};
