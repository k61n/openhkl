//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
#define OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H

#include "core/data/DataSet.h"
#include "core/shape/IPeakIntegrator.h"
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

class PeakComboBox;
class FoundPeakComboBox;
class DataComboBox;
class DetectorWidget;
class LinkedComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class Spoiler;
class SafeSpinBox;
class SafeDoubleSpinBox;
class ShapeComboBox;

//! Frame containing interface to integrate peaks
class SubframeIntegrate : public QWidget {
 public:
    SubframeIntegrate();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();
    //! Grab the refiner parameters
    void grabIntegrationParameters();

 private:
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();
    //! Set the refiner parameters
    void setIntegrationParameters();

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
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);

    //! Refresh the found peaks list
    void refreshTables();

    //! Do the integration
    void integrate();

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    PeakComboBox* _peak_combo;
    DataComboBox* _data_combo;

    // Integration region
    Spoiler* _integration_region_box;
    LinkedComboBox* _integration_region_type;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;

    //! Integration parameters
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
    ShapeComboBox* _shape_combo;
    QGroupBox* _discard_saturated;
    SafeDoubleSpinBox* _max_counts;

    QPushButton* _integrate_button;

    QStringList _cell_list;
    QStringList _predicted_list;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;
    ohkl::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    std::shared_ptr<ohkl::ShapeModelParameters> _shape_params;

    const std::map<std::string, ohkl::IntegratorType> _integrator_strings{
        {"Pixel sum integrator", ohkl::IntegratorType::PixelSum},
        {"Gaussian integrator", ohkl::IntegratorType::Gaussian},
        {"I/Sigma integrator", ohkl::IntegratorType::ISigma},
        {"1D Profile integrator", ohkl::IntegratorType::Profile1D},
        {"3D Profile integrator", ohkl::IntegratorType::Profile3D}};
};


#endif // OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
