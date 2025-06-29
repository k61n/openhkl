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

#include <QStandardItem>

namespace ohkl {
class Peak3D;
}
class PeakItemGraphic;

enum PeakDisplayModes { VALID, FILTER };

enum PeakColumn {
    h,
    k,
    l,
    d,
    px,
    py,
    Frame,
    SumIntensity,
    ProfileIntensity,
    SumSigma,
    ProfileSigma,
    SumStrength,
    ProfileStrength,
    SumBkg,
    ProfileBkg,
    SumBkgSigma,
    ProfileBkgSigma,
    BkgGradient,
    BkgGradientSigma,
    Scale,
    DataSet,
    uc,
    Rejection,
    Filtered,
    Enabled,
    Count,
};

enum Role { GetIndex = Qt::UserRole };

class PeakItem : public QStandardItem {
 public:
    PeakItem(ohkl::Peak3D* peak);
    ~PeakItem() = default;

    double peak_d() const;
    double sum_intensity() const;
    double sum_sigma() const;
    double profile_intensity() const;
    double profile_sigma() const;
    double sum_strength() const;
    double profile_strength() const;
    double sum_bkg() const;
    double sum_bkg_sigma() const;
    double profile_bkg() const;
    double profile_bkg_sigma() const;
    double bkg_gradient() const;
    double bkg_gradient_sigma() const;
    double scale() const;
    bool enabled() const;
    //! Retrieve the data of column and row
    QVariant peakData(const QModelIndex& index, int role, PeakDisplayModes mode) const;
    //! Get the peak pointer
    ohkl::Peak3D* peak() { return _peak; };
    //! Get the graphical representation
    PeakItemGraphic* peakGraphic() { return _peak_graphic.get(); };
    //! The column enumerators
    //! Whether the peak was caught by the filter
    bool caughtByFilter() const;

    int id() const { return _id; };

 private:
    //! pointer to the data
    ohkl::Peak3D* _peak;
    //! Generate the visual item
    std::unique_ptr<PeakItemGraphic> _peak_graphic;
    //! Unique id
    int _id;
    //! The last used id
    static unsigned long _last_id;
};

#endif // OHKL_GUI_ITEMS_PEAKITEM_H
