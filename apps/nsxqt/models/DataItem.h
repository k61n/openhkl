#ifndef DATAITEM_H
#define DATAITEM_H

#include "TreeItem.h"

class NumorItem;

namespace nsx
{
    namespace Instrument
    {
       class Experiment;
    }
}

class DataItem : public TreeItem
{
public:
    explicit DataItem(std::shared_ptr<Experiment> experiment);

    NumorItem *importData(std::shared_ptr<nsx::Data::DataSet> data);
    NumorItem* importData(const std::string& filename);
    NumorItem *importRawData(const std::vector<std::string>& filenames,
                             double wavelength, double delta_chi, double delta_omega, double delta_phi,
                             bool rowMajor, bool swapEndian, int bpp);

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
};

#endif // DATAITEM_H
