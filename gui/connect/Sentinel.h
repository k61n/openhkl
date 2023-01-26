//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/connect/Sentinel.h
//! @brief     Defines class Sentinel
//!
//! @homepage  https://openhkl.org
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

 public slots:

 signals:
    void instrumentStatesChanged();
};

#endif // GUI_CONNECT_SENTINEL_H
