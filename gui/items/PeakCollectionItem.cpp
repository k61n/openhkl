//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/items/PeakCollectionItem.cpp
//! @brief     The visual item of the peak with a pointer to the peak object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/items/PeakCollectionItem.h"

#include "base/geometry/ReciprocalVector.h"
#include "core/shape/PeakFilter.h"
#include "core/detector/Detector.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"


PeakCollectionItem::PeakCollectionItem()
{
    _peak_collection = nullptr;
    _mode = PeakDisplayModes::VALID;
}

PeakCollectionItem::PeakCollectionItem(nsx::PeakCollection* peak_collection)
{
    _peak_collection = peak_collection;
    _mode = PeakDisplayModes::VALID;

    std::vector<nsx::Peak3D*> peak_list = _peak_collection->getPeakList();
    for (nsx::Peak3D* peak : peak_list) {
        std::unique_ptr<PeakItem> item = std::make_unique<PeakItem>(peak);
        _peak_items.push_back(std::move(item));
    }
}

void PeakCollectionItem::setPeakCollection(nsx::PeakCollection* peak_collection)
{
    _peak_collection = peak_collection;

    std::vector<nsx::Peak3D*> peak_list = _peak_collection->getPeakList();
    _peak_items.clear();
    for (nsx::Peak3D* peak : peak_list) {
        std::unique_ptr<PeakItem> item = std::make_unique<PeakItem>(peak);
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
    if (_peak_collection == nullptr) {
        return 0;
    }

    return _peak_items.size();
}

int PeakCollectionItem::columnCount() const
{
    return 11;
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
        case Column::h: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                nsx::UnitCell* cell_1 = p1->peak()->unitCell();
                nsx::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    nsx::MillerIndex miller_index1(p1->peak()->q(), *cell_1);
                    nsx::MillerIndex miller_index2(p2->peak()->q(), *cell_2);
                    return (miller_index1[0] < miller_index2[0]);
                } else {
                    return ((cell_1 != nullptr) < (cell_2 != nullptr));
                }
            };
            break;
        }
        case Column::k: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                nsx::UnitCell* cell_1 = p1->peak()->unitCell();
                nsx::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    nsx::MillerIndex miller_index1(p1->peak()->q(), *cell_1);
                    nsx::MillerIndex miller_index2(p2->peak()->q(), *cell_2);
                    return (miller_index1[1] < miller_index2[1]);
                } else {
                    return ((cell_1 != nullptr) < (cell_2 != nullptr));
                }
            };
            break;
        }
        case Column::l: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                nsx::UnitCell* cell_1 = p1->peak()->unitCell();
                nsx::UnitCell* cell_2 = p2->peak()->unitCell();
                if (cell_1 && cell_2) {
                    nsx::MillerIndex miller_index1(p1->peak()->q(), *cell_1);
                    nsx::MillerIndex miller_index2(p2->peak()->q(), *cell_2);
                    return (miller_index1[2] < miller_index2[2]);
                } else {
                    return ((cell_1 != nullptr) < (cell_2 != nullptr));
                }
            };
            break;
        }
        case Column::px: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[0] < center2[0]);
            };
            break;
        }
        case Column::py: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[1] < center2[1]);
            };
            break;
        }
        case Column::Frame: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[2] < center2[2]);
            };
            break;
        }
        case Column::Intensity: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double intensity1 = p1->peak()->correctedIntensity().value();
                double intensity2 = p2->peak()->correctedIntensity().value();
                return (intensity1 < intensity2);
            };
            break;
        }
        case Column::Sigma: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double sigma_intensity1 = p1->peak()->correctedIntensity().sigma();
                double sigma_intensity2 = p2->peak()->correctedIntensity().sigma();
                return (sigma_intensity1 < sigma_intensity2);
            };
            break;
        }
        case Column::Strength: {
            compareFn = [](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double strength1 = p1->peak()->correctedIntensity().strength();
                double strength2 = p2->peak()->correctedIntensity().strength();
                return (strength1 < strength2);
            };
            break;
        }
        case Column::Numor: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                int numor_1 = p1->peak()->data()->reader()->metadata().key<int>("Numor");
                int numor_2 = p2->peak()->data()->reader()->metadata().key<int>("Numor");
                return (numor_1 < numor_2);
            };
            break;
        }
        case Column::uc: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                nsx::UnitCell* uc_1 = p1->peak()->unitCell();
                nsx::UnitCell* uc_2 = p2->peak()->unitCell();
                std::string uc_1Name = uc_1 ? uc_1->name() : "";
                std::string uc_2Name = uc_2 ? uc_2->name() : "";
                return (uc_2Name < uc_1Name);
            };
            break;
        }
        case Column::d: {
            compareFn = [&](std::unique_ptr<PeakItem>& p1, std::unique_ptr<PeakItem>& p2) {
                double d_1 = 1.0 / (p1->peak()->q().rowVector().norm());
                double d_2 = 1.0 / (p2->peak()->q().rowVector().norm());
                return (d_1 < d_2);
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
    std::vector<PeakItem*> output;
    for (int i = 0; i < _peak_items.size(); ++i) {
        output.push_back(_peak_items.at(i).get());
    }

    return output;
}

int PeakCollectionItem::numberOfPeaks(void) const
{
    return _peak_items.size();
}

int PeakCollectionItem::numberCaughtByFilter(void) const
{
    int n_caught = 0;
    for (int i = 0; i < _peak_items.size(); ++i) {
        if (_peak_items.at(i)->caughtByFilter()) {
            ++n_caught;
        }
    }
    return n_caught;
}

int PeakCollectionItem::numberRejectedByFilter(void) const
{
    return _peak_items.size() - numberCaughtByFilter();
}
