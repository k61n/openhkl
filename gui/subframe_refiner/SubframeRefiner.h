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
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace nsx {
class PeakCollection;
}
class DetectorWidget;
class LinkedComboBox;
class RefinerTables;
class Spoiler;
class SXPlot;
class PeakViewWidget;
class PlotCheckBox;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface to refine cell parameters and intstrument states
class SubframeRefiner : public QWidget {
 public:
    SubframeRefiner();

    //! Refresh all the inputs
    void refreshAll();

 private:
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();
    //! Set the refiner flags and constraints
    void setRefinerFlagsUp();
    //! Set up combo/button to update predicted peaks
    void setUpdateUp();
    //! Update the local copy of peaks to be refined, and the peak items and models
    void updatePeaks();
    //! Set up PeakViewWidgets
    void setPeakViewWidgetUp(PeakViewWidget* peak_widget, QString name);
    //! Refresh the detector scene
    void refreshPeakVisual();

    //! Grab the refiner parameters
    void grabRefinerParameters();
    //! Set the refiner parameters
    void setRefinerParameters();

    //! Refresh all inputs
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

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();


    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! Number of peaks updated
    int _n_updated;
    //! Whether the refinement succeeded
    bool _refine_success;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    LinkedComboBox* _exp_combo;
    LinkedComboBox* _peak_combo;
    LinkedComboBox* _data_combo;
    LinkedComboBox* _cell_combo;
    QCheckBox* _batch_cell_check;
    SafeSpinBox* _n_batches_spin;
    SafeSpinBox* _max_iter_spin;

    // refinement flags
    QComboBox* _residual_combo;
    QCheckBox* _refineUB;
    QCheckBox* _refineSamplePosition;
    QCheckBox* _refineSampleOrientation;
    QCheckBox* _refineDetectorPosition;
    QCheckBox* _refineKi;

    // Peak view widgets
    PeakViewWidget* _peak_view_widget_1;
    PeakViewWidget* _peak_view_widget_2;

    // plot widget
    Spoiler* _plot_box;

    // Detector scene
    DetectorWidget* _detector_widget;

    // Peak collections, items and models for refined and unrefined peaks
    nsx::PeakCollection* _refined_peaks;
    PeakCollectionItem _refined_collection_item;
    PeakCollectionModel _refined_model;
    nsx::PeakCollection _unrefined_peaks;
    PeakCollectionItem _unrefined_collection_item;
    PeakCollectionModel _unrefined_model;

    // update prediction
    LinkedComboBox* _predicted_combo;

    QPushButton* _refine_button;
    QPushButton* _update_button;

    QStringList _peak_list;
    QStringList _cell_list;
    QStringList _predicted_list;

    RefinerTables* _tables_widget;

    SXPlot* _plot_widget;

    QRandomGenerator _rng;

    //! Saved direct beam positions
    std::vector<nsx::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<nsx::DetectorEvent> _old_direct_beam_events;

    // Convert enum class ResidualType to a string
    const std::map<std::string, nsx::ResidualType> _residual_strings {
        {"Reciprocal space", nsx::ResidualType::QSpace},
        {"Real space", nsx::ResidualType::RealSpace}};
};


#endif // NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
