//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_refiner/SubframeIntegrate.h
//! @brief     Defines class SubframeIntegrate
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
#define NSX_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H

#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

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

class DetectorView;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class Spoiler;

//! Dialog to refine unit cell and instrument state
class SubframeIntegrate : public QWidget {
 public:
    SubframeIntegrate();

 public:
    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorView* getDetectorView() { return _figure_view; }

 private:
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();
    //! Grab the refiner parameters
    void grabIntegrationParameters();
    //! Set the refiner parameters
    void setIntegrationParameters() const;

    //! Refresh all the inputs
    void updateExptList();
    //! Update peak collection list on experiment change
    void updatePeakList();
    //! Update the data set for the DetectorScene/Peak table
    void updateDataset(const QString& dataname);
    //! Update the data list on experment change
    void updateDatasetList();
    //! Update the unit cell list on experment change
    void updateUnitCellList();
    //! Set up reintegration spoiler
    void setIntegrateUp();
    //! Set up the peak view widget
    void setPreviewUp();
    //! Set up the DetectorScene
    void setFigureUp();
    //! Refresh the DetctorScene
    void refreshPeakVisual();
    //! Set up the peak table
    void setPeakTableUp();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Wrapper for integration
    void runIntegration();
    //! Open the shape builder dialog
    void openShapeBuilder();
    //! Update the peak collection with the shape collection
    void refreshShapeStatus();
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);

    //! Refresh the found peaks list
    void refreshTables();

    //! Do the integration
    void integrate();

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

 private:
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! Parameters for shape library
    nsx::ShapeCollectionParameters _shape_params;
    //! Parameters for reintegration
    nsx::IntegrationParameters _integration_params;

 private:
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    QComboBox* _exp_combo;
    QComboBox* _peak_combo;
    QComboBox* _int_peak_combo;
    QComboBox* _data_combo;

    Spoiler* _integrate_box;
    QComboBox* _integrator_combo;
    QComboBox* _interpolation_combo;
    QSpinBox* _min_neighbours;
    QDoubleSpinBox* _radius;
    QDoubleSpinBox* _n_frames;
    QDoubleSpinBox* _peak_end;
    QDoubleSpinBox* _bkg_begin;
    QDoubleSpinBox* _bkg_end;
    QDoubleSpinBox* _radius_int;
    QDoubleSpinBox* _n_frames_int;
    QCheckBox* _fit_center;
    QCheckBox* _fit_covariance;

    QPushButton* _integrate_button;
    QPushButton* _build_shape_lib_button;

    QStringList _cell_list;
    QStringList _predicted_list;

    PeakViewWidget* _peak_view_widget;

    DetectorView* _figure_view;
    QSpinBox* _figure_spin;
    QScrollBar* _figure_scroll;

    PeakTableView* _peak_table;
    nsx::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;
};


#endif // NSX_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
