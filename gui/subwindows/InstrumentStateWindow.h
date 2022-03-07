//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subwindows/InstrumentStateWindow.cpp
//! @brief     Implements class InstrumentStateWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
#define NSX_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H

#include <QDialog>


//! Modeless dialog to display current instrument states
class InstrumentStateWindow : public QDialog {
 public:
    InstrumentStateWindow(QWidget* parent = nullptr);

    //! Overload QDialog::showEvent to resize window at runtime
    void showEvent(QShowEvent* event);
    //! Refresh the whole dialog
    void refreshAll();

};

#endif // NSX_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
