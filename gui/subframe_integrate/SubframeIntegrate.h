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
#include "core/shape/IPeakIntegrator.h"
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

class DetectorWidget;
class LinkedComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class Spoiler;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface to integrate peaks
class SubframeIntegrate : public QWidget {
 public:
    SubframeIntegrate();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();

 private:
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();
    //! Grab the refiner parameters
    void grabIntegrationParameters();
    //! Set the refiner parameters
    void setIntegrationParameters();

    //! Refresh all the inputs
    void updateExptList();
    //! Update peak collection list on experiment change
    void updatePeakList();
    //! Update the data list on experment change
    void updateDatasetList();
    //! Update the unit cell list on experment change
    void updateUnitCellList();
    //! Set up integration region spoiler
    void setIntegrationRegionUp();
    //! Set up integration spoiler
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
    //! Assign shapes to a peak collection
    void assignPeakShapes();
    //! Remove overlapping peaks
    void removeOverlappingPeaks();
    //! Wrapper for integration
    void runIntegration();
    //! Open the shape builder dialog
    void openShapeBuilder();
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);

    //! Refresh the found peaks list
    void refreshTables();

    //! Do the integration
    void integrate();

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    LinkedComboBox* _exp_combo;
    LinkedComboBox* _peak_combo;
    LinkedComboBox* _int_peak_combo;
    LinkedComboBox* _data_combo;

    // Integration region
    Spoiler* _integration_region_box;
    QCheckBox* _fixed_integration_region;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;

    Spoiler* _integrate_box;
    QComboBox* _integrator_combo;
    QComboBox* _interpolation_combo;
    SafeSpinBox* _min_neighbours;
    SafeDoubleSpinBox* _radius;
    SafeDoubleSpinBox* _n_frames;
    QCheckBox* _remove_overlaps;
    SafeDoubleSpinBox* _radius_int;
    SafeDoubleSpinBox* _n_frames_int;
    QCheckBox* _fit_center;
    QCheckBox* _fit_covariance;

    QPushButton* _integrate_button;
    QPushButton* _build_shape_lib_button;
    QPushButton* _assign_peak_shapes;

    QStringList _cell_list;
    QStringList _predicted_list;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;
    nsx::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    std::shared_ptr<nsx::ShapeCollectionParameters> _shape_params;

    const std::map<std::string, nsx::IntegratorType> _integrator_strings{
        {"Pixel sum integrator", nsx::IntegratorType::PixelSum},
        {"Gaussian integrator", nsx::IntegratorType::Gaussian},
        {"I/Sigma integrator", nsx::IntegratorType::ISigma},
        {"1D Profile integrator", nsx::IntegratorType::Profile1D},
        {"3D Profile integrator", nsx::IntegratorType::Profile3D}};
};


#endif // NSX_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
