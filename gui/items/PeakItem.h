//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/items/PeakItem.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_ITEMS_PEAKITEM_H
#define GUI_ITEMS_PEAKITEM_H

#include "core/peak/Peak3D.h"
#include <QStandardItem>

enum Column { 
        h, k, l, px, py, frame, 
        intensity, sigmaIntensity, 
        numor, unitCell, d, count };

class PeakItem : public QStandardItem {

public: 
    enum Column { h, k, l, px, py, Frame, Intensity, Sigma, Numor, uc, d, Count };

    PeakItem(nsx::Peak3D* peak);
    ~PeakItem() = default;
    //! Retrieve the data of column and row
    QVariant peakData(const QModelIndex &index, int role ) const;

private:
    //! pointer to the data
    nsx::Peak3D* _peak;
};

#endif // GUI_ITEMS_PEAKITEM_H