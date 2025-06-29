//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_rescale/SubframeRescale.h
//! @brief     Defines class SubframeRescale
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_RESCALE_SUBFRAMERESCALE_H
#define OHKL_GUI_SUBFRAME_RESCALE_SUBFRAMERESCALE_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QWidget>

class PeakComboBox;
class PeakTableView;
class QCheckBox;
class QComboBox;
class QPushButton;
class QSplitter;
class QVBoxLayout;
class SafeDoubleSpinBox;
class SafeSpinBox;
class ScienceSpinBox;
class SXPlot;

//! Frame containing interface to rescale peaks
class SubframeRescale : public QWidget {
    Q_OBJECT
 public:
    SubframeRescale();

    //! Refresh all the panels
    void refreshAll();

    //! Set the GUI rescaler parameters
    void grabRescalerParameters();
    //! Transfer rescaler parameters from GUI to core
    void setRescalerParameters();

 private:
    //! Set up the data selection GUI
    void setDataUp();
    //! Set the rescaling GUI up
    void setRescalerUp();
    //! Set up the peak table
    void setPeakTableUp();
    //! Set up the scale factor plot
    void setPlotUp();

    //! Refresh the QComboBox for the space group
    void refreshSpaceGroupCombo();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Disable widgets that are unsafe without relevant data
    void toggleUnsafeWidgets();

    //! Run the rescaler
    void rescale();
    //! Plot the scale factors as a function of image number
    void plot();

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    PeakComboBox* _peak_combo;
    QComboBox* _space_group_combo;

    // Merge parameters
    QCheckBox* _friedel_check;
    QCheckBox* _profile_intensity_check;

    // Refiner parameters
    ScienceSpinBox* _ftol_spin;
    ScienceSpinBox* _xtol_spin;
    ScienceSpinBox* _ctol_spin;
    SafeSpinBox* _max_iter_spin;
    SafeDoubleSpinBox* _frame_ratio_spin;
    QPushButton* _rescale_button;

    // Peak Table
    PeakTableView* _peak_table;
    ohkl::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    // Plot
    SXPlot* _plot;

    QVector<double> _image_numbers;
    QVector<double> _scale_factors;
    QVector<double> _convergence_step;
    QVector<double> _convergence;
};


#endif // OHKL_GUI_SUBFRAME_RESCALE_SUBFRAMERESCALE_H
