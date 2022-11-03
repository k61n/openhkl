//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/items/PeakItem.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_ITEMS_PEAKITEM_H
#define OHKL_GUI_ITEMS_PEAKITEM_H

#include "core/peak/Peak3D.h"

#include "gui/graphics_items/PeakItemGraphic.h"

#include <QStandardItem>

enum PeakDisplayModes { VALID, FILTER };

enum PeakColumn {
    h,
    k,
    l,
    px,
    py,
    Frame,
    Intensity,
    Sigma,
    Strength,
    BkgGradient,
    BkgGradientSigma,
    Numor,
    uc,
    d,
    Rejection,
    Filtered,
    Selected,
    Count,
};

class PeakItem : public QStandardItem {
 public:
    PeakItem(ohkl::Peak3D* peak);
    ~PeakItem() = default;

    double peak_d() const;
    double intensity() const;
    double sigma_intensity() const;
    double strength() const;
    double bkg_gradient() const;
    double bkg_gradient_sigma() const;
    bool selected() const;
    //! Retrieve the data of column and row
    QVariant peakData(const QModelIndex& index, int role, PeakDisplayModes mode) const;
    //! Get the peak pointer
    ohkl::Peak3D* peak() { return _peak; };
    //! Get the graphical representation
    PeakItemGraphic* peakGraphic() { return _peak_graphic.get(); };
    //! The column enumerators
    //! Whether the peak was caught by the filter
    bool caughtByFilter() const;

 private:
    //! pointer to the data
    ohkl::Peak3D* _peak;
    //! Generate the visual item
    std::unique_ptr<PeakItemGraphic> _peak_graphic;
};

#endif // OHKL_GUI_ITEMS_PEAKITEM_H
