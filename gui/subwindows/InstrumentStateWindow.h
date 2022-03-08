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
class QGridLayout;
class QSpinBox;
class QLineEdit;

//! Modeless dialog to display current instrument states
class InstrumentStateWindow : public QDialog {
 public:
    InstrumentStateWindow(QWidget* parent = nullptr);

    //! Set up the grids for instrument states
    void setStateGridsUp();

    //! Refresh list of datasets
    void updateData();
    //! Update the instrument states
    void updateState();
    //! Refresh the whole dialog
    void refreshAll();

 private:
    LinkedComboBox* _data_combo;
    QSpinBox* _frame_spin;
    QWidget* _instrument_state_widget;

    QVector<QVector<QLineEdit*>> _sample_orn_elements;
    QVector<QLineEdit*> _sample_pos_elements;
    QVector<QLineEdit*> _detector_pos_elements;
    QVector<QLineEdit*> _ki_elements;
};

#endif // NSX_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
