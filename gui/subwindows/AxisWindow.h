//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/AxisWindow.cph
//! @brief     Implements class AxisWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_AXIS_WINDOW_H
#define NSX_GUI_SUBWINDOWS_AXIS_WINDOW_H

#endif

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <QSize>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>

#include "gui/models/Session.h"

class AxisWindow : QDialog
{
    public:
        AxisWindow();
        refreshAll();

    private:
        QComboBox* _axis_selector;
        QLineEdit* _axis_name;
        QRadioButton* _axis_clockwise;
        QRadioButton* _axis_physical;
        QLineEdit* _x_axis_value;
        QLineEdit* _y_axis_value;
        QLineEdit* _z_axis_value;
};