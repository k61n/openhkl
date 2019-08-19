//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SideBar.h
//! @brief     Defines class SideBar
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SIDEBAR_H
#define GUI_PANELS_SIDEBAR_H

#include <QToolBar>

class SideBar : public QToolBar {
 public:
    SideBar(QWidget* parent = nullptr);

 private:
    void onHome();
    void onExperiment();
    void onFindPeaks();
    void onFilterPeaks();
    void onIndexer();
    void onInformation();

};

#endif // GUI_PANELS_SIDEBAR_H
