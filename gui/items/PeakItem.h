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

#ifndef NSX_GUI_ITEMS_PEAKITEM_H
#define NSX_GUI_ITEMS_PEAKITEM_H

#include "core/peak/Peak3D.h"

#include "gui/graphics_items/PeakItemGraphic.h"

#include <QStandardItem>

enum Column {
    h,
    k,
    l,
    px,
    py,
    frame,
    intensity,
    sigmaIntensity,
    strength,
    numor,
    unitCell,
    d,
    Selected,
    Count
};

enum PeakDisplayModes { VALID, FILTER };

class PeakItem : public QStandardItem {
 public:
    PeakItem(nsx::Peak3D* peak);
    ~PeakItem() = default;

 public:
    double peak_d() const;
    double intensity() const;
    double sigma_intensity() const;
    double strength() const;
    bool selected() const;
    //! Retrieve the data of column and row
    QVariant peakData(const QModelIndex& index, int role, PeakDisplayModes mode) const;
    //! Get the peak pointer
    nsx::Peak3D* peak() { return _peak; };
    //! Get the graphical representation
    PeakItemGraphic* peakGraphic();
    //! The column enumerators
    enum Column {
        h,
        k,
        l,
        px,
        py,
        Frame,
        Intensity,
        Sigma,
        Strength,
        Numor,
        uc,
        d,
        Selected,
        Count
    };
    //! Whether the peak was caught by the filter
    bool caughtByFilter(void) const;

 private:
    //! pointer to the data
    nsx::Peak3D* _peak;
};

#endif // NSX_GUI_ITEMS_PEAKITEM_H
