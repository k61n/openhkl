#pragma once

#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/PeakFinder.h>

#include "TreeItem.h"

class NumorItem;

class DataItem : public TreeItem
{
public:
    explicit DataItem();

    void importData();

    void importRawData();

    void importRawData(const std::vector<std::string>& filenames,
                             double wavelength, double delta_chi, double delta_omega, double delta_phi,
                             bool rowMajor, bool swapEndian, int bpp);

    void findPeaks();
    nsx::DataList selectedData();

    void convertToHDF5();

private:
    
    nsx::sptrPeakFinder _peakFinder;
};
