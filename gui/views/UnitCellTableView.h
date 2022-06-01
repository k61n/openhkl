//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/UnitCellTableView.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H
#define NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H

#include <QStandardItem>
#include <QString>
#include <QTableView>

class UnitCellTableView : public QTableView {
    Q_OBJECT
 public:
    explicit UnitCellTableView(QWidget* parent = 0);
};

//! Value-Tuple Item:
//! A QStandardItem which holds a floating-point value and its standard deviation or quality
//! measure. The sorting mechanism is taylored to sort according the value and then (if the values
//! are equal) according the std. devs. or quality measures. Sorting is based on the stored
//! numerical values, not the textual representation.
//! * Sorting examples:
//! ValueTupleItem("1.2(3)", 1.238, 3e-3) < ValueTupleItem("1.0(3)", 1.01, 3e-3)
//! ValueTupleItem("1.2(3)", 1.238, 3e-3) < ValueTupleItem("1.2(3)", 1.240, 3e-3)
//! ValueTupleItem("1.2(3)", 1.238, 3e-3) < ValueTupleItem("1.2(4)", 1.238, 4e-3)
class ValueTupleItem : public QStandardItem {
 public:
    explicit ValueTupleItem(const QString& text, const double val, const double qmes = 0);
    ~ValueTupleItem() = default;

    //! Custom sorting;
    //! See the Qt documentation for `QStandardItem::operator<`.
    bool operator<(const QStandardItem& other) const override;

 public:
    double value = 0;
    double qmeasure = 0;
};


#endif // NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H
