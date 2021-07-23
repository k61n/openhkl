//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/connect/Sentinel.h
//! @brief     Defines class Sentinel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef GUI_CONNECT_SENTINEL_H
#define GUI_CONNECT_SENTINEL_H

#include <QObject>

//! MainWin-owned object that controls signals to linked objects across widgets/windows
enum class ComboType;

class Sentinel : public QObject {
    Q_OBJECT
 public:
    Sentinel();

    //! Add an item to all combos of this type if there is no such object in the current object
    void addLinkedComboItem(const ComboType, const QString& item);
    //! Set the  item list of all combos of this type if there is no such object in the current
    //! object
    void setLinkedComboItems(const ComboType, const QStringList& list);

 public slots:
    //! When one combo is changed, update the others
    void onLinkedComboChanged(const ComboType combo_type, const int id, const QStringList& list);

 signals:
    void linkedComboChanged(const ComboType combo_type, const int id, const QStringList& list);
    void linkedComboItemAdded(const ComboType combo_type, const QString& item);
    void setLinkedComboList(const ComboType combo_type, const QStringList& list);
};

#endif // GUI_CONNECT_SENTINEL_H
