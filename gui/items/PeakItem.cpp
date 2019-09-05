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

PeakItem::PeakItem(nsx::Peak3D* peak)
    :QStandardItem()
{
    _peak = peak;
    _peak_graphic = std::unique_ptr<PeakItemGraphic>(new PeakItemGraphic(peak));
}

QVariant PeakItem::peakData(
    const QModelIndex &index, int role, PeakDisplayModes mode) const
{

    int col = index.column();
    
    Eigen::RowVector3i hkl = {0, 0, 0};
    Eigen::RowVector3d hkl_error = {0.0, 0.0, 0.0};

    nsx::sptrUnitCell cell = _peak->unitCell();

    if (cell) {
        nsx::MillerIndex miller_index(_peak->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            hkl = miller_index.rowVector();
            hkl_error = miller_index.error();
        }
    }
    
    double peak_d = 1.0 / (_peak->q().rowVector().norm());
    double intensity = _peak->correctedIntensity().value();
    double sigma_intensity = _peak->correctedIntensity().sigma();
    const Eigen::Vector3d& peak_center = _peak->shape().center();
    
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
                    return intensity;
                }
                case Column::Sigma: {
                    return sigma_intensity;
                }
                case Column::Numor: {
                    return _peak->data()->reader()->metadata().key<int>("Numor");
                }
                case Column::uc: {
                    nsx::sptrUnitCell unit_cell = _peak->unitCell();
                    if (unit_cell)
                        return QString::fromStdString(unit_cell->name());
                    else
                        return QString("not set");
                }
                case Column::d: {
                    return peak_d;
                }
            }
            break;

        case Qt::ForegroundRole: {
            if (!_peak->enabled())
                return QBrush(Qt::red);
            break;
        }
        case Qt::BackgroundColorRole: {
            switch(mode){
                case PeakDisplayModes::FILTER: {
                    if (_peak->caughtByFilter()){
                        return QBrush(Qt::darkGreen);
                    }else{
                        return QBrush(Qt::darkRed);
                    }
                }    
            break;
            }
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
