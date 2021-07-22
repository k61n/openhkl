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

enum class ComboType;

class Sentinel : public QObject {
    Q_OBJECT
 public:
    Sentinel();
 public slots:
    void onLinkedComboChanged(const ComboType combo_type, const int id, const QStringList& list);
 signals:
    void linkedComboChanged(const ComboType combo_type, const int id, const QStringList& list);
};

#endif // GUI_CONNECT_SENTINEL_H
