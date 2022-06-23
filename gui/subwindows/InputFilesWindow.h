//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InputFilesWindows.cpp
//! @brief     Implements class InputFilesWindows
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H
#define NSX_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H

 

#include <QDialog>
 
#include <QWidget>
#include <QTableWidget>
#include <QSize>

#include "gui/models/Session.h"

class InputFilesWindow : public QDialog {

 public:
    InputFilesWindow(QWidget* parent);

    //! Refresh the whole dialinputog
    void refreshAll(int id=-1); 

 private:
   QTableWidget* _files_table; 
};

#endif // NSX_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H
