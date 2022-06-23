//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InstrumentDataWindow.cpp
//! @brief     Implements class InstrumentDataWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_INSTRUMENTDATAWINDOW_H
#define NSX_GUI_SUBWINDOWS_INSTRUMENTDATAWINDOW_H

#include <QDialog>
#include <QComboBox>
#include <QTableWidget>

class InstrumentDataWindow : public QDialog{
    public:
        InstrumentDataWindow(QWidget* parent = nullptr);

        void refreshAll();
    //public slots:
    private:
        QComboBox* _instruments;
        QTableWidget* _table;
};

#endif //NSX_GUI_SUBWINDOWS_INSTRUMENTDATAWINDOW_H