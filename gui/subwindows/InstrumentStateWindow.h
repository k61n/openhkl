//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InstrumentStateWindow.cpp
//! @brief     Implements class InstrumentStateWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
#define OHKL_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H

#include <QDialog>

class DataComboBox;
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

 public slots:
    void onFrameChanged(int frame);

 private:
    DataComboBox* _data_combo;
    QSpinBox* _frame_spin;
    QWidget* _instrument_state_widget;

    QVector<QVector<QLineEdit*>> _sample_orn_elements;
    QVector<QLineEdit*> _sample_pos_elements;
    QVector<QLineEdit*> _detector_pos_elements;
    QVector<QLineEdit*> _ki_elements;
};

#endif // OHKL_GUI_SUBWINDOWS_INSTRUMENTSTATEWINDOW_H
