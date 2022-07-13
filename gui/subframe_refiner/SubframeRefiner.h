//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "core/shape/ShapeModel.h"
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

namespace ohkl {
class PeakCollection;
}
class CellComboBox;
class DataComboBox;
class PeakComboBox;
class DetectorWidget;
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
    //! Grab the refiner parameters
    void grabRefinerParameters();

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

    //! Set the refiner parameters
    void setRefinerParameters();

    //! set parameters for _n_batches spin box
    void setBatchesUp();
    //! set up the variables to plot
    void setPlotUp();

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

    //! Get a pointer to the cell combo
    CellComboBox* cellCombo() const { return _cell_combo; };


    //! The loaded data list
    std::vector<ohkl::sptrDataSet> _data_list;
    //! Number of peaks updated
    int _n_updated;
    //! Whether the refinement succeeded
    bool _refine_success;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    PeakComboBox* _peak_combo;
    DataComboBox* _data_combo;
    CellComboBox* _cell_combo;
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
    ohkl::PeakCollection* _refined_peaks;
    PeakCollectionItem _refined_collection_item;
    PeakCollectionModel _refined_model;
    ohkl::PeakCollection _unrefined_peaks;
    PeakCollectionItem _unrefined_collection_item;
    PeakCollectionModel _unrefined_model;

    // update prediction
    PeakComboBox* _predicted_combo;

    QPushButton* _refine_button;
    QPushButton* _update_button;

    QStringList _peak_list;
    QStringList _cell_list;
    QStringList _predicted_list;

    RefinerTables* _tables_widget;

    SXPlot* _plot_widget;

    QRandomGenerator _rng;

    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;

    // Convert enum class ResidualType to a string
    const std::map<std::string, ohkl::ResidualType> _residual_strings{
        {"Reciprocal space", ohkl::ResidualType::QSpace},
        {"Real space", ohkl::ResidualType::RealSpace}};
};


#endif // NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
