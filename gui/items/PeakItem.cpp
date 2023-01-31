//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/items/PeakItem.cpp
//! @brief     The visual item of the peak with a pointer to the peak object
//!
//! @homepage  https://openhkl.org
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
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

PeakItem::PeakItem(ohkl::Peak3D* peak)
{
    _peak = peak;
    _peak_graphic = std::unique_ptr<PeakItemGraphic>(new PeakItemGraphic(peak));
}

double PeakItem::peak_d() const
{
    try {
        return 1.0 / (_peak->q().rowVector().norm());
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::sum_intensity() const
{
    try {
        return _peak->sumIntensity().value();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::sum_sigma() const
{
    try {
        return _peak->sumIntensity().sigma();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::profile_intensity() const
{
    try {
        return _peak->profileIntensity().value();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::profile_sigma() const
{
    try {
        return _peak->profileIntensity().sigma();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::sum_strength() const
{
    try {
        return _peak->sumIntensity().strength();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::profile_strength() const
{
    try {
        return _peak->profileIntensity().strength();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::bkg_gradient() const
{
    try {
        return _peak->meanBkgGradient().value();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

double PeakItem::bkg_gradient_sigma() const
{
    try {
        return _peak->meanBkgGradient().sigma();
    } catch (std::range_error& e) {
        return 0.0;
    }
}

bool PeakItem::enabled() const
{
    return _peak->enabled();
}

QVariant PeakItem::peakData(const QModelIndex& index, int role, PeakDisplayModes mode) const
{
    int col = index.column();

    const Eigen::Vector3d& peak_center = _peak->shape().center();
    const ohkl::MillerIndex miller_index = _peak->hkl();
    Eigen::RowVector3i hkl = miller_index.rowVector();
    Eigen::RowVector3d hkl_error = miller_index.error();

    switch (role) {
        case Qt::DisplayRole:

            switch (col) {
                case PeakColumn::h: {
                    return hkl(0);
                }
                case PeakColumn::k: {
                    return hkl(1);
                }
                case PeakColumn::l: {
                    return hkl(2);
                }
                case PeakColumn::px: {
                    return peak_center(0);
                }
                case PeakColumn::py: {
                    return peak_center(1);
                }
                case PeakColumn::Frame: {
                    return peak_center(2);
                }
                case PeakColumn::SumIntensity: {
                    return sum_intensity();
                }
                case PeakColumn::SumSigma: {
                    return sum_sigma();
                }
                case PeakColumn::ProfileIntensity: {
                    return profile_intensity();
                }
                case PeakColumn::ProfileSigma: {
                    return profile_sigma();
                }
                case PeakColumn::SumStrength: {
                    return sum_strength();
                }
                case PeakColumn::ProfileStrength: {
                    return profile_strength();
                }
                case PeakColumn::BkgGradient: {
                    return bkg_gradient();
                }
                case PeakColumn::BkgGradientSigma: {
                    return bkg_gradient_sigma();
                }
                case PeakColumn::DataSet: {
                    return QString::fromStdString(_peak->dataSet()->name());
                }
                case PeakColumn::uc: {
                    const ohkl::UnitCell* unit_cell = _peak->unitCell();
                    if (!unit_cell)
                        return QString("not set");
                    return QString::fromStdString(unit_cell->name());
                }
                case PeakColumn::d: {
                    return peak_d();
                }
                case PeakColumn::Rejection: {
                    return QString::fromStdString(_peak->rejectionString());
                }
                case PeakColumn::Filtered: {
                    return QString::number(_peak->caughtByFilter());
                }
            }
            break;

        case Qt::CheckStateRole: {
            if (index.column() == PeakColumn::Enabled) {
                if (_peak->enabled())
                    return Qt::Checked;
                else
                    return Qt::Unchecked;
            }
            break;
        }

        case Qt::ForegroundRole: {
            if (!_peak->enabled())
                return QBrush(Qt::red);
            break;
        }
        case Qt::BackgroundRole: {
            switch (mode) {
                case PeakDisplayModes::FILTER: {
                    if (_peak->caughtByFilter())
                        return QBrush(Qt::darkGreen);
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
                case PeakColumn::h: return hkl[0] + hkl_error[0];
                case PeakColumn::k: return hkl[1] + hkl_error[1];
                case PeakColumn::l: return hkl[2] + hkl_error[2];
            }
            break;
    }
    return QVariant::Invalid;
}

bool PeakItem::caughtByFilter() const
{
    return _peak->caughtByFilter();
}
