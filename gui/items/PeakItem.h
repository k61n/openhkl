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

#include "gui/graphics_items/PeakItemGraphic.h"

#include <QStandardItem>

enum Column { 
        h, k, l, px, py, frame, 
        intensity, sigmaIntensity, 
        numor, unitCell, d, count };

enum PeakDisplayModes { VALID, FILTER };

class PeakItem : public QStandardItem {

public: 
    PeakItem(nsx::Peak3D* peak);
    ~PeakItem() = default;

public:

    //! Retrieve the data of column and row
    QVariant peakData(const QModelIndex &index, int role, PeakDisplayModes mode ) const;
    //! Get the peak pointer
    nsx::Peak3D* peak() {return _peak;};
    //! Get the graphical representation
    PeakItemGraphic* peakGraphic() {return _peak_graphic.get();};
    //!The column enumerators
    enum Column { h, k, l, px, py, Frame, Intensity, Sigma, Numor, uc, d, Count };

private:

    //! pointer to the data
    nsx::Peak3D* _peak;
    //! Generate the visual item
    std::unique_ptr<PeakItemGraphic> _peak_graphic;
};

#endif // GUI_ITEMS_PEAKITEM_H