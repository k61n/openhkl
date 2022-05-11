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
#include "core/shape/Profile3D.h"
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

namespace nsx
{
class Peak3D;
}

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
class QGraphicsView;

//! Frame containing interface to integrate peaks
class SubframeShapes : public QWidget {
 public:
    SubframeShapes();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();

 public slots:
    void onPeakSelected(nsx::Peak3D* peak);

 private:
    //! Select dataset, peak collection, set parameters
    void setInputUp();
    //! Select the peak to preview, generate mean covariance for selected peak or whole collection
    void setComputeShapesUp();
    //! Set up assignment of shapes to a peak collection
    void setAssignShapesUp();
    //! Grab the refiner parameters
    void grabShapeParameters();
    //! Set the refiner parameters
    void setShapeParameters();

    //! Set up the peak view widget
    void setPreviewUp();
    //! Set up the widget to preview the shape
    void setShapePreviewUp();
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
    //! Save the shape collection
    void saveShapes();

    //! Refresh the found peaks list
    void refreshTables();

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

    // Preview/Compute shapes box
    SafeDoubleSpinBox* _x;
    SafeDoubleSpinBox* _y;
    SafeDoubleSpinBox* _frame;

    SafeSpinBox* _min_neighbours;
    SafeDoubleSpinBox* _pixel_radius;
    SafeDoubleSpinBox* _frame_radius;
    QComboBox* _interpolation_combo;

    QPushButton* _calculate_mean_profile;
    QPushButton* _save_shapes;

    //! Assign shapes box
    PeakComboBox* _predicted_combo;
    ShapeComboBox* _shape_combo;
    QPushButton* _assign_peak_shapes;

    //! Shape preview box
    QGridLayout* _shape_grid;
    QGraphicsView* _graphics_view;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;

    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    nsx::ShapeCollection _shape_collection;
    nsx::Profile3D _profile;
    nsx::Peak3D* _current_peak;

    QGroupBox* _shape_group;
};


#endif // NSX_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H
