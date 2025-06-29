//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/PeakComboBox.h
//! @brief     Defines class PeakComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_PEAKCOMBO_H
#define OHKL_GUI_UTILITY_PEAKCOMBO_H

#include <QComboBox>

namespace ohkl {
class PeakCollection;
}

enum class ComboType { PeakCollection, FoundPeaks, PredictedPeaks, IntegratedPeaks };

using PeakList = std::vector<ohkl::PeakCollection*>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class PeakComboBox : public QComboBox {
    Q_OBJECT

 public:
    PeakComboBox(QWidget* parent = nullptr);
    ~PeakComboBox();

    //! Add a PeakCollection via its pointer
    void addPeakCollection(ohkl::PeakCollection* peaks);

    //! Add a list of peak collection
    void addPeakCollections(const PeakList& peaks);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current peak collection
    ohkl::PeakCollection* currentPeakCollection() const;

    //! Refresh the combo box text
    void refresh();

    //! Set combo to have an empty first entry
    void setEmptyFirst();

    //! Refresh all combos of this type
    void refreshAll();

 private:
    //! Index-sorted list of pointers to peak collections
    static PeakList _peak_collections;
    //! Vector of all instances to refresh all in one call
    static QVector<PeakComboBox*> _all_combos;

 protected:
    //! pointer to the peak list for the current combo type
    PeakList* _list_pointer;

    //! Current peak collection
    QString _current;

    //! First position in combo is empty
    bool _empty_first = false;
};

#endif // OHKL_GUI_UTILITY_PEAKCOMBO_H
