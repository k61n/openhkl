#ifndef NSXQT_DATAITEM_H
#define NSXQT_DATAITEM_H

#include <nsxlib/data/IData.h>

#include "TreeItem.h"

class NumorItem;

namespace nsx
{
class Experiment;
}

class DataItem : public TreeItem
{
public:
    explicit DataItem(std::shared_ptr<nsx::Experiment> experiment);

    NumorItem *importData(std::shared_ptr<nsx::DataSet> data);
    NumorItem* importData(const std::string& filename);
    NumorItem *importRawData(const std::vector<std::string>& filenames,
                             double wavelength, double delta_chi, double delta_omega, double delta_phi,
                             bool rowMajor, bool swapEndian, int bpp);

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
};

#endif // NSXQT_DATAITEM_H
