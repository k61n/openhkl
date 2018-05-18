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

    NumorItem* importData(const std::string& filename);
    NumorItem *importRawData(const std::vector<std::string>& filenames,
                             double wavelength, double delta_chi, double delta_omega, double delta_phi,
                             bool rowMajor, bool swapEndian, int bpp);

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

    void findPeaks();
    nsx::DataList selectedData();

private:
    NumorItem *importData(nsx::sptrDataSet data);
    nsx::sptrPeakFinder _peakFinder;
};
