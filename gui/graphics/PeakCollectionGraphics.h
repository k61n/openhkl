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
#include "gui/models/ColorMap.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QGraphicsPixmapItem>
#include <QVector>
#include <opencv2/core/types.hpp>

namespace ohkl {
class PeakCenterDataSet;
class Peak3D;
}
class PeakItemGraphic;
class PeakCenterGraphic;

//! Container for settings and pointers for visualising peaks
class PeakCollectionGraphics {

 public:
    PeakCollectionGraphics(PeakCollectionModel* model);

    //! Get the peak model pointer
    PeakCollectionModel* peakModel() const { return _peak_model; };
    //! Get the DataSet pointer associated with the PeakCollection
    ohkl::sptrDataSet dataSet() const { return _peak_model->dataSet(); };
    //! Set the integration parameters
    void setIntegrationParams(const ohkl::IntegrationParameters& params) {_params = params; };
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
    void setPerFrameSpots(std::vector<std::vector<cv::KeyPoint>>* points) {
        _per_frame_spots = points;
    };

    //! Set the integration region peak and background colours
    void setIntRegionColors(const QColor& peak, const QColor& bkg) {
        _peakPxColor = peak;
        _bkgPxColor = bkg;
    };
    //! Set the 3rd party peak circle colour and size
    void setExtPeakCircles(const QColor& color, int size) {
        _3rdparty_color = color;
        _3rdparty_size = size;
    };

    //! Get a QImage of the integration region mask
    QImage* getIntegrationRegionImage(std::size_t frame_idx, ohkl::Peak3D* peak = nullptr);
    //! Set integration region overlay parameters from PeakWidget
    void initIntRegionFromPeakWidget(const PeakViewWidget::Set& set);

    bool peaksEnabled() const { return _peaks_enabled; };
    bool intRegionsEnabled() const { return _int_regions_enabled; };
    bool extPeaksEnabled() const { return _ext_peaks_enabled; };
    bool detectorSpotsEnabled() const { return _detector_spots_enabled; };
    void setPeaksEnabled(bool enabled) { _peaks_enabled = enabled; };
    void setIntRegionEnabled(bool enabled) { _int_regions_enabled = enabled; };
    void setExtPeaksEnabled(bool enabled) { _ext_peaks_enabled = enabled; };
    void setDetectorSpotsEnabled(bool enabled) { _detector_spots_enabled = enabled; };

 private:
    //! Generate a mask of integration regions (a matrix of integers classifying pixels)
    void getIntegrationMask(Eigen::MatrixXi& mask, std::size_t frame_idx);
    //! Generate a mask for a single peak only
    void getSinglePeakIntegrationMask(
        ohkl::Peak3D* peak, Eigen::MatrixXi& mask, std::size_t frame_idx);
    //! Populate vector of 3rd party peak centers
    void link3rdPartyPeaks(ohkl::PeakCenterDataSet* pcd);
    //! Refresh the model data
    void peakModelDataChanged();
    //! Plot settings for 3rd party peak centres
    void setup3rdPartyPeaks(bool draw, const QColor& color, int size);

    //! Whether the peaks will be generated
    bool _peaks_enabled;
    //! Whether the integration regions will be generated
    bool _int_regions_enabled;
    //! Whether external peaks will be generated
    bool _ext_peaks_enabled;
    //! Whether detector spots will be generated
    bool _detector_spots_enabled;
    //! Integration parameters for determining peak shapes
    ohkl::IntegrationParameters _params;
    //! The peak model
    PeakCollectionModel* _peak_model;
    //! Pointer to peak selected in DetectorScene
    ohkl::Peak3D* _selected_peak;
    //! Externally generated peak data
    ohkl::PeakCenterDataSet* _peak_center_data;
    //! Detector spots found using OpenCV
    std::vector<std::vector<cv::KeyPoint>>* _per_frame_spots;

    //! Colour of peak pixels in integration region
    QColor _peakPxColor;
    //! Colour of background pixels in integration region
    QColor _bkgPxColor;
    //! Toggle preview of integration region rather than using regions defined from peaks
    bool _preview_int_regions;
    //! Integration region types
    ohkl::RegionType _int_region_type;
    //! Integration Region bounds
    double _peak_end;
    double _bkg_begin;
    double _bkg_end;
    double _fixed_peak_end;
    double _fixed_bkg_begin;
    double _fixed_bkg_end;

    //! Colour of 3rd party peaks
    QColor _3rdparty_color;
    //! Size of 3rd party peaks
    int _3rdparty_size;
};

#endif // OHKL_GUI_GRAPHICS_PEAKCOLLECTIONGRAPHICS_H
