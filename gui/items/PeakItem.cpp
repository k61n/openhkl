//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/items/PeakItem.cpp
//! @brief     The visual item of the peak with a pointer to the peak object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/items/PeakItem.h"

#include "base/geometry/ReciprocalVector.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

PeakItem::PeakItem(nsx::Peak3D* peak) : QStandardItem()
{
    _peak = peak;
}

double PeakItem::peak_d() const
{
    try {
        return 1.0 / (_peak->q().rowVector().norm());
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::intensity() const
{
    try {
        return _peak->correctedIntensity().value();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::sigma_intensity() const
{
    try {
        return _peak->correctedIntensity().sigma();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::strength() const
{
    try {
        return _peak->correctedIntensity().strength();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

bool PeakItem::selected() const
{
    return _peak->selected();
}

QVariant PeakItem::peakData(const QModelIndex& index, int role, PeakDisplayModes mode) const
{
    int col = index.column();

    const Eigen::Vector3d& peak_center = _peak->shape().center();
    const nsx::MillerIndex miller_index = _peak->hkl();
    Eigen::RowVector3i hkl = miller_index.rowVector();
    Eigen::RowVector3d hkl_error = miller_index.error();

    switch (role) {
        case Qt::DisplayRole:

            switch (col) {
                case Column::h: {
                    return hkl(0);
                }
                case Column::k: {
                    return hkl(1);
                }
                case Column::l: {
                    return hkl(2);
                }
                case Column::px: {
                    return peak_center(0);
                }
                case Column::py: {
                    return peak_center(1);
                }
                case Column::Frame: {
                    return peak_center(2);
                }
                case Column::Intensity: {
                    return intensity();
                }
                case Column::Sigma: {
                    return sigma_intensity();
                }
                case Column::Strength: {
                    return strength();
                }
                case Column::Numor: {
                    return _peak->dataSet()->reader()->metadata().key<int>("Numor");
                }
                case Column::uc: {
                    const nsx::UnitCell* unit_cell = _peak->unitCell();
                    if (unit_cell)
                        return QString::fromStdString(unit_cell->name());
                    else
                        return QString("not set");
                }
                case Column::d: {
                    return peak_d();
                }
                case Column::Selected: {
                    return selected();
                }
            }
            break;

        case Qt::ForegroundRole: {
            if (!_peak->selected())
                return QBrush(Qt::red);
            break;
        }
        case Qt::BackgroundRole: {
            switch (mode) {
                case PeakDisplayModes::FILTER: {
                    if (_peak->caughtByFilter())
                        return QBrush(Qt::darkGreen);
                    else
                        return QBrush(Qt::darkRed);
                }
                case PeakDisplayModes::VALID: {
                    return QBrush();
                }
            }
            break;
        }
        case Qt::ToolTipRole:
            switch (col) {
                case Column::h: return hkl[0] + hkl_error[0];
                case Column::k: return hkl[1] + hkl_error[1];
                case Column::l: return hkl[2] + hkl_error[2];
            }
            break;
    }
    return QVariant::Invalid;
}

bool PeakItem::caughtByFilter(void) const
{
    return _peak->caughtByFilter();
}

PeakItemGraphic* PeakItem::peakGraphic()
{
    return new PeakItemGraphic(_peak);
}
