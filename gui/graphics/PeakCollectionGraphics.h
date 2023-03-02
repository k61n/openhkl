//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/PeakCollectionGraphics.h
//! @brief     Defines class PeakCollectionGraphics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_PEAKCOLLECTIONGRAPHICS_H
#define OHKL_GUI_GRAPHICS_PEAKCOLLECTIONGRAPHICS_H

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "gui/graphics/DetectorSceneParams.h"
#include "gui/models/ColorMap.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QGraphicsPixmapItem>
#include <QVector>

namespace ohkl {
class KeyPointCollection;
class PeakCenterDataSet;
class Peak3D;
}
class PeakItemGraphic;
class PeakCenterGraphic;

enum class VisualisationType { Enabled, Filtered };

//! Container for settings and pointers for visualising peaks
class PeakCollectionGraphics {
 public:
    PeakCollectionGraphics(DetectorSceneParams* params);

    //! Set the peak model pointer
    void setPeakModel(PeakCollectionModel* model) { _peak_model = model; };
    //! Set the peak view widget, used to set colours etc
    void setPeakViewWidget(PeakViewWidget* widget) { _peak_view_widget = widget; };
    //! Get the peak view widget
    PeakViewWidget* peakViewWidget() const { return _peak_view_widget; };
    //! Get the peak model pointer
    PeakCollectionModel* peakModel() const { return _peak_model; };
    //! Get the DataSet pointer associated with the PeakCollection
    ohkl::sptrDataSet dataSet() const { return _peak_model->dataSet(); };
    //! Set the integration parameters
    void setIntegrationParams(const ohkl::IntegrationParameters& params) { _int_params = params; };
    //! Set the visualisation type to enabled or filtered
    void setVisualisationType(VisualisationType vtype) { _visual_type = vtype; };
    //! Generate the PeakItemGraphics and return them in a vector. The parameters
    //! Are used to construct the integration regions
    QVector<PeakItemGraphic*> peakItemGraphics(std::size_t frame_idx);
    //! Generate peak centers (not shapes) from 3rd party data
    QVector<PeakCenterGraphic*> extPeakGraphics(std::size_t frame_idx);
    //! Generate peak centers (not shapes) from detector spots via OpenCV
    QVector<PeakCenterGraphic*> detectorSpots(std::size_t frame_idx);

    //! Set data for peaks imported from external code
    void setExtPeakData(ohkl::PeakCenterDataSet* pcd) { _peak_center_data = pcd; };
    //! Set data for detector spots located using OpenCV
    void setKeyPoints(ohkl::KeyPointCollection* points)
    {
        _keypoint_collection = points;
    };

    //! Set the integration region peak and background colours
    void setIntRegionColors(const QColor& peak, const QColor& bkg)
    {
        _peakPxColor = peak;
        _bkgPxColor = bkg;
    };
    //! Set the 3rd party peak circle colour and size
    void setSpotCircles(const QColor& color, int size)
    {
        _spot_color = color;
        _spot_size = size;
    };

    //! Get a QImage of the integration region mask
    QImage* getIntegrationRegionImage(std::size_t frame_idx, ohkl::Peak3D* peak = nullptr);
    //! Set integration region overlay parameters from PeakWidget
    void initIntRegionFromPeakWidget();

 private:
    //! Generate a mask of integration regions (a matrix of integers classifying pixels)
    void getIntegrationMask(Eigen::MatrixXi& mask, std::size_t frame_idx);
    //! Generate a mask for a single peak only
    void getSinglePeakIntegrationMask(
        ohkl::Peak3D* peak, Eigen::MatrixXi& mask, std::size_t frame_idx);
    //! Determine the visual type of the peak
    bool visualType(ohkl::Peak3D* peak);

    //! DetectorScene parameters to determine what is displayed
    DetectorSceneParams* _params;
    //! Integration parameters for determining peak shapes
    ohkl::IntegrationParameters _int_params;
    //! The peak model
    PeakCollectionModel* _peak_model;
    //! Pointer to peak selected in DetectorScene
    ohkl::Peak3D* _selected_peak;
    //! Externally generated peak data
    ohkl::PeakCenterDataSet* _peak_center_data;
    //! Detector spots found using OpenCV
    ohkl::KeyPointCollection* _keypoint_collection;
    //! The widget used to set visual parameters (color, size etc)
    PeakViewWidget* _peak_view_widget;
    //! Type of peak visualisation (separate by validity or filter status)
    VisualisationType _visual_type;

    //! Colour of peak pixels in integration region
    QColor _peakPxColor;
    //! Colour of background pixels in integration region
    QColor _bkgPxColor;
    //! Toggle preview of integration region rather than using regions defined from peaks
    bool _preview_int_regions;

    //! Colour of detector spots
    QColor _spot_color;
    //! Size of detector spots
    int _spot_size;
};

#endif // OHKL_GUI_GRAPHICS_PEAKCOLLECTIONGRAPHICS_H
