//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/items/PeakCollectionItem.cpp
//! @brief     The visual item of the peak with a pointer to the peak object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/items/PeakCollectionItem.h"

#include "base/geometry/ReciprocalVector.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

PeakCollectionItem::PeakCollectionItem()
{
    _peak_collection = nullptr;
    _mode = PeakDisplayModes::VALID;
}

PeakCollectionItem::PeakCollectionItem(const ohkl::PeakCollection* peak_collection)
{
    _peak_collection = peak_collection;
    _mode = PeakDisplayModes::VALID;

    std::vector<ohkl::Peak3D*> peak_list = _peak_collection->getPeakList();
    for (ohkl::Peak3D* peak : peak_list) {
        auto item = std::make_unique<PeakItem>(peak);
        _peak_items.push_back(std::move(item));
    }
}

void PeakCollectionItem::setPeakCollection(const ohkl::PeakCollection* peak_collection)
{
    if (!peak_collection) {
        throw std::runtime_error(
            "Error PeakCollectionItem::setPeakCollection: No valid PeakCollection was given");
    }
    _peak_collection = peak_collection;

    std::vector<ohkl::Peak3D*> peak_list = _peak_collection->getPeakList();
    _peak_items.clear();
    for (ohkl::Peak3D* peak : peak_list) {
        auto item = std::make_unique<PeakItem>(peak);
        _peak_items.push_back(std::move(item));
    }
}

std::string PeakCollectionItem::name() const
{
    std::string name = "No peak collection";

    if (!(_peak_collection == nullptr))
        return _peak_collection->name();

    return name;
}

int PeakCollectionItem::childCount() const
{
    if (_peak_collection == nullptr)
        return 0;

    return _peak_items.size();
}

int PeakCollectionItem::columnCount() const
{
    return static_cast<int>(PeakColumn::Count);
}

QVariant PeakCollectionItem::data(const QModelIndex& index, int role) const
{
    return peakItemAt(index.row())->peakData(index, role, _mode);
}

PeakItem* PeakCollectionItem::peakItemAt(int row) const
{
    return _peak_items.at(row).get();
}

void PeakCollectionItem::sort(int column, Qt::SortOrder order)
{
    std::function<bool(std::unique_ptr<PeakItem>&, std::unique_ptr<PeakItem>&)> compareFn =
        [](std::unique_ptr<PeakItem>&, std::unique_ptr<PeakItem>&) { return false; };

    switch (column) {
        case PeakColumn::h: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const ohkl::UnitCell* cell_1 = p1->peak()->unitCell();
                const ohkl::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    return (p1->peak()->hkl().h() < p2->peak()->hkl().h());
                }
                return ((cell_1 != nullptr) < (cell_2 != nullptr));
            };
            break;
        }
        case PeakColumn::k: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const ohkl::UnitCell* cell_1 = p1->peak()->unitCell();
                const ohkl::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    return (p1->peak()->hkl().k() < p2->peak()->hkl().k());
                }
                return ((cell_1 != nullptr) < (cell_2 != nullptr));
            };
            break;
        }
        case PeakColumn::l: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const ohkl::UnitCell* cell_1 = p1->peak()->unitCell();
                const ohkl::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    return (p1->peak()->hkl().l() < p2->peak()->hkl().l());
                }
                return ((cell_1 != nullptr) < (cell_2 != nullptr));
            };
            break;
        }
        case PeakColumn::px: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[0] < center2[0]);
            };
            break;
        }
        case PeakColumn::py: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[1] < center2[1]);
            };
            break;
        }
        case PeakColumn::Frame: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[2] < center2[2]);
            };
            break;
        }
        case PeakColumn::SumIntensity: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double intensity1 = 0.0;
                double intensity2 = 0.0;
                try {
                    intensity1 = p1->peak()->correctedSumIntensity().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    intensity2 = p2->peak()->correctedSumIntensity().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (intensity1 < intensity2);
            };
            break;
        }
        case PeakColumn::SumSigma: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double sigma_intensity1 = 0.0;
                double sigma_intensity2 = 0.0;
                try {
                    sigma_intensity1 = p1->peak()->correctedSumIntensity().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    sigma_intensity2 = p2->peak()->correctedSumIntensity().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (sigma_intensity1 < sigma_intensity2);
            };
            break;
        }
        case PeakColumn::ProfileIntensity: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double intensity1 = 0.0;
                double intensity2 = 0.0;
                try {
                    intensity1 = p1->peak()->correctedProfileIntensity().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    intensity2 = p2->peak()->correctedProfileIntensity().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (intensity1 < intensity2);
            };
            break;
        }
        case PeakColumn::ProfileSigma: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double sigma_intensity1 = 0.0;
                double sigma_intensity2 = 0.0;
                try {
                    sigma_intensity1 = p1->peak()->correctedProfileIntensity().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    sigma_intensity2 = p2->peak()->correctedProfileIntensity().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (sigma_intensity1 < sigma_intensity2);
            };
            break;
        }
        case PeakColumn::SumStrength: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double strength1 = 0.0;
                double strength2 = 0.0;
                try {
                    strength1 = p1->peak()->correctedSumIntensity().strength();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    strength2 = p2->peak()->correctedSumIntensity().strength();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (strength1 < strength2);
            };
            break;
        }
        case PeakColumn::ProfileStrength: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double strength1 = 0.0;
                double strength2 = 0.0;
                try {
                    strength1 = p1->peak()->correctedProfileIntensity().strength();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    strength2 = p2->peak()->correctedProfileIntensity().strength();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (strength1 < strength2);
            };
            break;
        }
        case PeakColumn::BkgGradient: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double gradient1 = 0.0;
                double gradient2 = 0.0;
                try {
                    gradient1 = p1->peak()->meanBkgGradient().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    gradient2 = p2->peak()->meanBkgGradient().value();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (gradient1 < gradient2);
            };
            break;
        }
        case PeakColumn::BkgGradientSigma: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double sigma1 = 0.0;
                double sigma2 = 0.0;
                try {
                    sigma1 = p1->peak()->meanBkgGradient().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    sigma2 = p2->peak()->meanBkgGradient().sigma();
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (sigma1 < sigma2);
            };
            break;
        }
        case PeakColumn::DataSet: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const std::string data_1 = p1->peak()->dataSet()->name();
                const std::string data_2 = p2->peak()->dataSet()->name();
                return (data_1 < data_2);
            };
            break;
        }
        case PeakColumn::uc: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const ohkl::UnitCell* uc_1 = p1->peak()->unitCell();
                const ohkl::UnitCell* uc_2 = p2->peak()->unitCell();
                const std::string uc_1Name = uc_1 ? uc_1->name() : "";
                const std::string uc_2Name = uc_2 ? uc_2->name() : "";
                return (uc_2Name < uc_1Name);
            };
            break;
        }
        case PeakColumn::d: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double d_1 = 0.0;
                double d_2 = 0.0;
                try {
                    d_1 = 1.0 / (p1->peak()->q().rowVector().norm());
                } catch (std::range_error& e) {
                    // interpolation error
                }
                try {
                    d_2 = 1.0 / (p2->peak()->q().rowVector().norm());
                } catch (std::range_error& e) {
                    // interpolation error
                }
                return (d_1 < d_2);
            };
            break;
        }
        case PeakColumn::Rejection: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                int s_1 = static_cast<int>(p1->peak()->rejectionFlag());
                int s_2 = static_cast<int>(p2->peak()->rejectionFlag());
                return (s_1 < s_2);
            };
            break;
        }
        case PeakColumn::Enabled: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                bool e_1 = p1->peak()->enabled();
                bool e_2 = p2->peak()->enabled();
                return (e_1 < e_2);
            };
            break;
        }
        case PeakColumn::Filtered: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                bool s_1 = p1->peak()->caughtByFilter();
                bool s_2 = p2->peak()->caughtByFilter();
                return (s_1 < s_2);
            };
            break;
        }
    }

    std::sort(_peak_items.begin(), _peak_items.end(), compareFn);

    if (order == Qt::DescendingOrder)
        std::reverse(_peak_items.begin(), _peak_items.end());
}

int PeakCollectionItem::returnRowOfVisualItem(PeakItemGraphic* peak_graphic) const
{
    for (int i = 0; i < _peak_items.size(); i++) {
        if (peak_graphic == _peak_items.at(i)->peakGraphic())
            return i;
    }
    return 0;
}

void PeakCollectionItem::setFilterMode()
{
    _mode = PeakDisplayModes::FILTER;
}

std::vector<PeakItem*> PeakCollectionItem::peakItems() const
{
    std::vector<PeakItem*> output(_peak_items.size());
    for (int i = 0; i < _peak_items.size(); ++i)
        output[i] = _peak_items.at(i).get();
    return output;
}

int PeakCollectionItem::numberOfPeaks() const
{
    return _peak_items.size();
}

int PeakCollectionItem::numberCaughtByFilter() const
{
    int n_caught = 0;
    for (int i = 0; i < _peak_items.size(); ++i) {
        if (_peak_items.at(i)->caughtByFilter())
            ++n_caught;
    }
    return n_caught;
}

int PeakCollectionItem::numberRejectedByFilter() const
{
    return _peak_items.size() - numberCaughtByFilter();
}

void PeakCollectionItem::reset()
{
    _peak_collection = nullptr;
    _peak_items.clear();
}
