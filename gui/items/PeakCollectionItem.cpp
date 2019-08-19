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
#include "core/analyse/PeakFilter.h"
#include "core/detector/Detector.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

PeakCollectionItem::PeakCollectionItem(nsx::PeakCollection* peak_collection)
    :QStandardItem()
{
    _peak_collection = peak_collection;

    std::vector<nsx::Peak3D*>* peak_list = _peak_collection->getPeakList();
    for (nsx::Peak3D* peak : *peak_list)
    {
        PeakItem* item = new PeakItem(peak); 
        _peak_items.push_back(item);
    }
}

int PeakCollectionItem::childCount() const
{
    return _peak_items.size();
}

int PeakCollectionItem::columnCount() const
{
    return 11;
}

QVariant PeakCollectionItem::data(
    const QModelIndex &index, 
    int role) const
{
    return peakItemAt(index.row())->peakData(index, role);
}

PeakItem* PeakCollectionItem::peakItemAt(int row) const
{
    return _peak_items.at(row);
}

void PeakCollectionItem::sort(int column, Qt::SortOrder order)
{
    std::function<bool(PeakItem*, PeakItem*)> compareFn =
        [](PeakItem*, PeakItem*) { return false; };

    switch (column) {
        case Column::h: {
            compareFn = [&](PeakItem* p1, PeakItem* p2) {
                nsx::sptrUnitCell cell_1 = p1->peak()->unitCell();
                nsx::sptrUnitCell cell_2 = p2->peak()->unitCell();
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
            compareFn = [&](PeakItem* p1, PeakItem* p2) {
                nsx::sptrUnitCell cell_1 = p1->peak()->unitCell();
                nsx::sptrUnitCell cell_2 = p2->peak()->unitCell();
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
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                nsx::sptrUnitCell cell_1 = p1->peak()->unitCell();
                nsx::sptrUnitCell cell_2 = p2->peak()->unitCell();
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
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[0] < center2[0]);
            };
            break;
        }
        case Column::py: {
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[1] < center2[1]);
            };
            break;
        }
        case Column::Frame: {
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                const Eigen::Vector3d& center1 = p1->peak()->shape().center();
                const Eigen::Vector3d& center2 = p2->peak()->shape().center();
                return (center1[2] < center2[2]);
            };
            break;
        }
        case Column::Intensity: {
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                double intensity1 = p1->peak()->correctedIntensity().value();
                double intensity2 = p2->peak()->correctedIntensity().value();
                return (intensity1 < intensity2);
            };
            break;
        }
        case Column::Sigma: {
            compareFn = [](PeakItem* p1, PeakItem* p2) {
                double sigma_intensity1 = p1->peak()->correctedIntensity().sigma();
                double sigma_intensity2 = p2->peak()->correctedIntensity().sigma();
                return (sigma_intensity1 < sigma_intensity2);
            };
            break;
        }
        case Column::Numor: {
            compareFn = [&](PeakItem* p1, PeakItem* p2) {
                int numor_1 = p1->peak()->data()->reader()->metadata().key<int>("Numor");
                int numor_2 = p2->peak()->data()->reader()->metadata().key<int>("Numor");
                return (numor_1 < numor_2);
            };
            break;
        }
        case Column::uc: {
            compareFn = [&](PeakItem* p1, PeakItem* p2) {
                nsx::sptrUnitCell uc_1 = p1->peak()->unitCell();
                nsx::sptrUnitCell uc_2 = p2->peak()->unitCell();
                std::string uc_1Name = uc_1 ? uc_1->name() : "";
                std::string uc_2Name = uc_2 ? uc_2->name() : "";
                return (uc_2Name < uc_1Name);
            };
            break;
        }
        case Column::d: {
            compareFn = [&](PeakItem* p1, PeakItem* p2) {
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
    for (int i = 0; i < _peak_items.size(); i++){
        if (peak_graphic ==_peak_items.at(i)->peakGraphic())
            return i;
    }
    return 0;
}
