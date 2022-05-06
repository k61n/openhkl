//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_shapes/SubframeShapes.h
//! @brief     Defines class SubframeShapes
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H
#define NSX_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H

#include "core/data/DataSet.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/utility/PredictedPeakComboBox.h"

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
#include <qgroupbox.h>

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

//! Frame containing interface to integrate peaks
class SubframeShapes : public QWidget {
 public:
    SubframeShapes();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();

 private:
    //! Select dataset, peak collection, set parameters
    void setInputUp();
    //! Select the peak to preview, generate mean covariance for selected peak or whole collection
    void setAssignShapesUp();
    //! Grab the refiner parameters
    void grabShapeParameters();
    //! Set the refiner parameters
    void setShapeParameters();

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
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);

    //! Generate the shape collection
    void buildShapeCollection();
    //! Compute the mean profile at the given coordinates
    void computeProfile();

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

    // Generate shapes box
    SafeSpinBox* _nx;
    SafeSpinBox* _ny;
    SafeSpinBox* _nz;

    QGroupBox* _kabsch;

    SafeDoubleSpinBox* _sigma_d;
    SafeDoubleSpinBox* _sigma_m;

    SafeDoubleSpinBox* _min_strength;
    SafeDoubleSpinBox* _min_d;
    SafeDoubleSpinBox* _max_d;

    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;

    QPushButton* _build_collection;

    // Preview/Assign shapes box
    PeakComboBox* _predicted_combo;
    SafeDoubleSpinBox* _x;
    SafeDoubleSpinBox* _y;
    SafeDoubleSpinBox* _frame;

    SafeSpinBox* _min_neighbours;
    SafeDoubleSpinBox* _pixel_radius;
    SafeDoubleSpinBox* _frame_radius;
    QComboBox* _interpolation_combo;

    QPushButton* _calculate_mean_profile;

    QPushButton* _assign_peak_shapes;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;

    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    std::unique_ptr<nsx::ShapeCollection> _shape_collection;
    std::shared_ptr<nsx::ShapeCollectionParameters> _shape_params;
};


#endif // NSX_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H
