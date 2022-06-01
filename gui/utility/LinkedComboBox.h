//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/LinkedComboBox.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_LINKEDCOMBO_H
#define NSX_GUI_UTILITY_LINKEDCOMBO_H

#include <QComboBox>

class Sentinel;

enum class ComboType {
    Experiment,
    DataSet,
    UnitCell,
    PeakCollection,
    FoundPeaks,
    PredictedPeaks,
    RegionType,
    Count
};

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class LinkedComboBox : public QComboBox {
    Q_OBJECT

 public:
    LinkedComboBox(ComboType combo_type, Sentinel* sentinel, QWidget* parent = nullptr);

    //! When the list for one combo is set, set it for all others
    void updateList(const QStringList& list);

 public slots:
    //! Set the item list of any combo of the given type, without the given id
    void onComboChanged(ComboType combo_type, int id, const QStringList& list);
    //! Add an item to all linked combos of this type
    void onItemAdded(ComboType combo_type, const QString& item);
    //! Set the item list of all linked combos of this type
    void onSetItems(ComboType combo_type, const QStringList& list);

 signals:
    void comboChanged(const ComboType combo_type, int id, const QStringList& list);

 private:
    //! The type of this combo box
    ComboType _combo_type;
    //! integer id for a LinkedComboBox instance
    int _id;
    //! For setting the instance id number
    static int _count;
};

#endif // NSX_GUI_UTILITY_LINKEDCOMBO_H
