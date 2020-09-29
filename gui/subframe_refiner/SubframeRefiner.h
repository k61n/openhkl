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

#include "core/shape/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/utility/ColorButton.h"
#include "gui/views/PeakTableView.h"

#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

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
    //! Refine button
    void setRefineUp();
    //! Construct the table
    void setRefinerTableUp();

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

    //! Refresh the found peaks list
    void refreshTable();
    //! Refresh the found peaks visual properties
    void refreshVisual();

    //! Do the refinement
    void refine();

 private:
    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;

 private:
    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    Spoiler* _input_box;

    QComboBox* _exp_combo;
    QComboBox* _peak_combo;
    QComboBox* _data_combo;
    QComboBox* _cell_combo;
    QSpinBox* _n_batches_spin;

    Spoiler* _refiner_flags_box;

    QCheckBox* _refineUB;
    QCheckBox* _refineSamplePosition;
    QCheckBox* _refineSampleOrientation;
    QCheckBox* _refineDetectorOrientation;
    QCheckBox* _refineKi;

    SpoilerCheck* _UB_box;
    SpoilerCheck* _sample_position_box;
    SpoilerCheck* _sample_orientation_box;
    SpoilerCheck* _ki_box;

    QPushButton* _refine_button;
    QPushButton* _save_button;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    QStringList _peak_list;
    QStringList _cell_list;
};


#endif // NSX_GUI_SUBFRAME_REFINER_SUBFRAMEREFINER_H
