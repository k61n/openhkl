//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
    PeakCollection,
    UnitCell,
    Count
};

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class LinkedComboBox : public QComboBox {
    Q_OBJECT

 public:
    LinkedComboBox(ComboType combo_type, Sentinel* sentinel, QWidget* parent = nullptr);

    void updateList(const QStringList& list);

 public slots:
    void onComboChanged(ComboType combo_type, int id, const QStringList& list);

 signals:
    void comboChanged(const ComboType combo_type, int id, const QStringList& list);

 private:
    //! integer id for a LinkedComboBox instance
    ComboType _combo_type;
    int _id;

    static int _count;

};

#endif // NSX_GUI_UTILITY_LINKEDCOMBO_H
