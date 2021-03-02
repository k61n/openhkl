//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_refiner/SubframeRefiner.h
//! @brief     Defines class SubframeRefiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
#define NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H

#include "core/algo/Refiner.h"
#include "core/shape/ShapeCollection.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace nsx {
class PeakCollection;
}
class RefinerTables;
class Spoiler;
class SXPlot;
class PlotCheckBox;

//! Dialog to refine unit cell and instrument state
class SubframeRefiner : public QWidget {
 public:
    SubframeRefiner();
    ~SubframeRefiner();

 public:
    //! Refresh all the inputs
    void refreshAll();

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();
    //! Set the refiner flags and constraints
    void setRefinerFlagsUp();
    //! Update the predicted peaks
    void setUpdateUp();

 private:
    //! Grab the refiner parameters
    void grabRefinerParameters();
    //! Set the refiner parameters
    void setRefinerParameters() const;

    //! Refresh all the inputs
    void setParametersUp();
    //! Refresh all the inputs
    void updateExptList();
    //! Update peak collection list on experiment change
    void updatePeakList();
    //! Update the data list on experment change
    void updateDatasetList();
    //! Update the unit cell list on experment change
    void updateUnitCellList();
    //! set parameters for _n_batches spin box
    void setBatchesUp();
    //! set up the variables to plot
    void setPlotUp();
    //! Update list of predicted peak collections
    void updatePredictedList();
    //! Set up reintegration spoiler
    void setReintegrateUp();
    //! Open the shape builder dialog
    void openShapeBuilder();
    //! Update the peak collection with the shape collection
    void refreshPeakShapeStatus();
    //! Wrapper for reintegration
    void runReintegration(nsx::PeakCollection* peaks);
    //! Reintegrate found peaks
    void reintegrateFound();
    //! Reintegrate predicted peaks
    void reintegratePredicted();

    //! Refresh the found peaks list
    void refreshTables();

    //! Do the refinement
    void refine();
    //! Update predicted peaks
    void updatePredictions();

    //! Set up the lattice parameter table
    void setLatticeTableUp();

 private:
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! Number of peaks updated
    int _n_updated;
    //! Whether the refinement succeeded
    bool _refine_success;
    //! Parameters for refinement/reintegration
    nsx::RefinerParameters _refiner_params;
    //! Parameters for shape library
    nsx::ShapeCollectionParameters _shape_params;
    //! Parameters for reintegration
    nsx::IntegrationParameters _integration_params;

 private:
    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    Spoiler* _input_box;

    QComboBox* _exp_combo;
    QComboBox* _peak_combo;
    QComboBox* _data_combo;
    QComboBox* _cell_combo;
    QSpinBox* _n_batches_spin;

    // refinement flags
    Spoiler* _refiner_flags_box;

    QCheckBox* _refineUB;
    QCheckBox* _refineSamplePosition;
    QCheckBox* _refineSampleOrientation;
    QCheckBox* _refineDetectorPosition;
    QCheckBox* _refineKi;

    // plot widget
    Spoiler* _plot_box;

    QVector<PlotCheckBox*> _plot_check_boxes;

    // update prediction
    QPushButton* _refine_button;

    Spoiler* _update_box;
    QComboBox* _predicted_combo;
    QPushButton* _update_button;

    // reintegration
    Spoiler* _reintegrate_box;
    QPushButton* _build_shape_lib;
    QComboBox* _integrator_combo;
    QDoubleSpinBox* _radius;
    QDoubleSpinBox* _n_frames;
    QDoubleSpinBox* _peak_end_int;
    QDoubleSpinBox* _bkg_start_int;
    QDoubleSpinBox* _bkg_end_int;
    QDoubleSpinBox* _radius_int;
    QDoubleSpinBox* _n_frames_int;
    QCheckBox* _fit_center;
    QCheckBox* _fit_covariance;
    QPushButton* _reintegrate_found;
    QPushButton* _reintegrate_predicted;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    QStringList _peak_list;
    QStringList _cell_list;
    QStringList _predicted_list;

    RefinerTables* _tables_widget;

    SXPlot* _plot_widget;

    QRandomGenerator _rng;

    void refreshPlot();
};


#endif // NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
