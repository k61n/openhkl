//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InputFilesWindows.cpp
//! @brief     Implements class InputFilesWindows
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H
#define OHKL_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H


#include <QDialog>

#include <QComboBox>
#include <QSize>
#include <QTableWidget>
#include <QWidget>

#include "gui/models/Session.h"

class InputFilesWindow : public QDialog {

 public:
    InputFilesWindow(QWidget* parent);

    void setDataset(QString set);

    void refreshAll();

 public slots:
    void on_combobox_select();

 private:
    QTableWidget* _files_table;
    QComboBox* _data_set;
};

#endif // OHKL_GUI_SUBWINDOWS_INPUT_FILES_WINDOWS_H
