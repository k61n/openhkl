//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_shapes/SubframeShapes.h
//! @brief     Defines class SubframeShapes
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H
#define OHKL_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H

#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QWidget>
#include <memory>

class PeakComboBox;
class DataComboBox;
class DetectorWidget;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class SafeSpinBox;
class SafeDoubleSpinBox;
class ShapeComboBox;
class QCheckBox;
class QComboBox;
class QGraphicsView;
class QGroupBox;
class QPushButton;
class QSplitter;
class QVBoxLayout;
namespace ohkl {
class Peak3D;
class RegionData;
class ShapeModel;
struct ShapeModelParameters;
}

//! Frame containing interface to integrate peaks
class SubframeShapes : public QWidget {
 public:
    SubframeShapes();

    //! Refresh all the inputs
    void refreshAll();
    //! Grab the refiner parameters
    void grabShapeParameters();
    //! Set the refiner parameters
    void setShapeParameters();

    //! detector view
    DetectorWidget* detectorWidget();

 public slots:
    void onPeakSelected(ohkl::Peak3D* peak);
    void onShapeChanged();
    void onRegionModeChanged();

 private:
    //! Select dataset, peak collection, set parameters
    void setInputUp();
    //! Select the peak to preview, generate mean covariance for selected peak or whole collection
    void setComputeShapesUp();

    //! Set up the peak view widget
    void setPreviewUp();
    //! Set up the widget to preview the shape
    void setShapePreviewUp();
    //! Set up the DetectorScene
    void setFigureUp();
    //! Set up the peak table
    void setPeakTableUp();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Assign shapes to a peak collection
    void assignPeakShapes();
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);

    //! Generate the shape collection
    void buildShapeModel();
    //! Compute the mean profile at the given coordinates
    void computeProfile();
    //! Convert profile matrices to image
    void regionData2Image(ohkl::RegionData* region_data);
    //! Generate a peak for preview in in DetectorScene
    void getPreviewPeak(ohkl::Peak3D* selected_peak);
    //! Save the shape collection
    void saveShapes();

    //! Get a pointer to the selected shape model
    ohkl::ShapeModel* shapeModel();

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
    SafeSpinBox* _nsubdiv;

    QGroupBox* _kabsch;

    SafeDoubleSpinBox* _sigma_d;
    SafeDoubleSpinBox* _sigma_m;

    SafeDoubleSpinBox* _min_strength;
    SafeDoubleSpinBox* _min_d;
    SafeDoubleSpinBox* _max_d;
    SafeSpinBox* _max_width;

    QCheckBox* _show_single_region;
    QComboBox* _integration_region_type;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;

    QPushButton* _build_collection;

    // Preview/Compute shapes box
    SafeDoubleSpinBox* _x;
    SafeDoubleSpinBox* _y;
    SafeDoubleSpinBox* _frame;

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
    QGraphicsView* _image_view;
    QGraphicsView* _profile_view;
    QGraphicsPixmapItem* _peak_pixmap;
    QGraphicsPixmapItem* _profile_pixmap;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;

    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    std::unique_ptr<ohkl::ShapeModel> _shape_model;
    ohkl::Peak3D* _current_peak;
    std::unique_ptr<ohkl::Peak3D> _preview_peak;

    QGroupBox* _shape_group;
};


#endif // OHKL_GUI_SUBFRAME_SHAPES_SUBFRAMESHAPES_H
