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

#include "gui/utility/LinkedComboBox.h"
#include <QDialog>

class LinkedComboBox;

//! Modeless dialog to display current instrument states
class InstrumentStateWindow : public QDialog {
 public:
    InstrumentStateWindow(QWidget* parent = nullptr);

    //! Refresh list of datasets
    void updateData();
    //! Refresh the whole dialog
    void refreshAll();

 private:
    LinkedComboBox* _data_combo;

};

#endif // NSX_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
