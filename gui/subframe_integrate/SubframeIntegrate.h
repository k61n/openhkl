//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_refiner/SubframeIntegrate.h
//! @brief     Defines class SubframeIntegrate
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
#define OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H

#include "core/integration/IIntegrator.h"
#include "core/peak/Peak3D.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QSortFilterProxyModel>
#include <QWidget>

class PeakComboBox;
class DataComboBox;
class DetectorWidget;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QPushButton;
class QSplitter;
class QVBoxLayout;
class Spoiler;
class SafeSpinBox;
class SafeDoubleSpinBox;
class ShapeComboBox;
namespace ohkl {
class PeakCollection;
struct ShapeModelParameters;
enum class RejectionFlag;
}

//! Frame containing interface to integrate peaks
class SubframeIntegrate : public QWidget {
    Q_OBJECT
 public:
    SubframeIntegrate();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();
    //! Grab the refiner parameters
    void grabIntegrationParameters();
    //! Set the refiner parameters
    void setIntegrationParameters();

 public slots:
    void onGradientSettingsChanged();

 signals:
    void signalGradient(int kernel);

 private:
    //! Select experiment, dataset, peak collection, unit cell
    void setInputUp();

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
    //! Set up the peak table
    void setPeakTableUp();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Assign shapes to a peak collection
    void assignPeakShapes();
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
    QComboBox* _integration_region_type;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;

    //! Integration parameters
    Spoiler* _integrate_box;
    QComboBox* _integrator_combo;
    QComboBox* _interpolation_combo;
    SafeDoubleSpinBox* _radius;
    SafeDoubleSpinBox* _n_frames;
    QCheckBox* _remove_overlaps;
    QCheckBox* _remove_masked;
    SafeDoubleSpinBox* _radius_int;
    SafeDoubleSpinBox* _n_frames_int;
    QCheckBox* _fit_center;
    QCheckBox* _fit_covariance;
    ShapeComboBox* _shape_combo;
    QGroupBox* _discard_saturated;
    SafeDoubleSpinBox* _max_counts;
    QGroupBox* _use_max_strength;
    SafeDoubleSpinBox* _max_strength;
    QGroupBox* _use_max_d;
    SafeDoubleSpinBox* _max_d;
    QGroupBox* _use_max_width;
    SafeSpinBox* _max_width;
    QGroupBox* _compute_gradient;
    QComboBox* _gradient_kernel;

    QPushButton* _integrate_button;

    QStringList _cell_list;
    QStringList _predicted_list;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;

    PeakTableView* _peak_table;
    ohkl::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;
    QSortFilterProxyModel _sort_proxy_model;

    std::map<ohkl::Peak3D*, ohkl::RejectionFlag> _overlap_saved_flags;
    std::map<ohkl::Peak3D*, ohkl::RejectionFlag> _mask_saved_flags;

    std::shared_ptr<ohkl::ShapeModelParameters> _shape_params;

    QVector<QWidget*> _widgets;

    const std::map<ohkl::IntegratorType, std::string> _integrator_strings{
        {ohkl::IntegratorType::PixelSum, "Pixel sum integrator"},
        {ohkl::IntegratorType::Gaussian, "Gaussian integrator"},
        {ohkl::IntegratorType::ISigma, "I/Sigma integrator"},
        {ohkl::IntegratorType::Profile1D, "1D Profile integrator"},
        {ohkl::IntegratorType::Profile3D, "3D Profile integrator"},
        {ohkl::IntegratorType::Shape, "Shape model integrator"}};
};


#endif // OHKL_GUI_SUBFRAME_INTEGRATE_SUBFRAMEINTEGRATE_H
