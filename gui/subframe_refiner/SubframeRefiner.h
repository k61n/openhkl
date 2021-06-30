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

    //! Refresh the found peaks list
    void refreshTables();

    //! Do the refinement
    void refine();
    //! Update predicted peaks
    void updatePredictions();

    //! Set up the lattice parameter table
    void setLatticeTableUp();

    //! Returns all plot check boxes
    QList<PlotCheckBox*> plotCheckBoxes() const;

    //! Refresh the plot (e.g. after changes of plot check boxes)
    void refreshPlot();

 private:
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! Number of peaks updated
    int _n_updated;
    //! Whether the refinement succeeded
    bool _refine_success;
    //! Parameters for refinement
    nsx::RefinerParameters _refiner_params;

 private:
    QVBoxLayout* _left_layout;

    // data selection
    QComboBox* _exp_combo;
    QComboBox* _peak_combo;
    QComboBox* _data_combo;
    QComboBox* _cell_combo;
    QSpinBox* _n_batches_spin;

    // refinement flags
    QCheckBox* _refineUB;
    QCheckBox* _refineSamplePosition;
    QCheckBox* _refineSampleOrientation;
    QCheckBox* _refineDetectorPosition;
    QCheckBox* _refineKi;

    // plot widget
    Spoiler* _plot_box;

    // update prediction
    QComboBox* _predicted_combo;

    QStringList _peak_list;
    QStringList _cell_list;
    QStringList _predicted_list;

    RefinerTables* _tables_widget;

    SXPlot* _plot_widget;

    QRandomGenerator _rng;
};


#endif // NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
