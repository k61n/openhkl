//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/PeakCollectionGraphics.cpp
//! @brief     Implements class PeakCollectionGraphics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/PeakCollectionGraphics.h"

#include "core/integration/IIntegrator.h"
#include "core/loader/XFileHandler.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/KeyPointCollection.h"
#include "gui/graphics_items/PeakCenterGraphic.h"
#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/items/PeakItem.h"
#include "gui/utility/ColorButton.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QVector>

#include <vector>

using EventType = ohkl::IntegrationRegion::EventType;

PeakCollectionGraphics::PeakCollectionGraphics(DetectorSceneParams* params)
    : _params(params)
    , _int_params()
    , _peak_model(nullptr)
    , _peak_view_widget(nullptr)
    , _visual_type(VisualisationType::Enabled)
    , _peakPxColor(QColor(255, 255, 0, 128)) // yellow, alpha = 0.5
    , _bkgPxColor(QColor(0, 255, 0, 128)) // green, alpha = 0.5
    , _spot_color(Qt::black)
    , _spot_size(10)
{
}

void PeakCollectionGraphics::initIntRegionFromPeakWidget()
{
    if (!_peak_view_widget)
        return;

    auto set = _peak_view_widget->set1;
    _preview_int_regions = set.previewIntRegion->isChecked();
    _int_params.region_type = static_cast<ohkl::RegionType>(set.regionType->currentIndex());
    if (_int_params.region_type == ohkl::RegionType::VariableEllipsoid) {
        _int_params.peak_end = set.peakEnd->value();
        _int_params.bkg_begin = set.bkgBegin->value();
        _int_params.bkg_end = set.bkgEnd->value();
    } else {
        _int_params.fixed_peak_end = set.peakEnd->value();
        _int_params.fixed_bkg_begin = set.bkgBegin->value();
        _int_params.fixed_bkg_end = set.bkgEnd->value();
    }
    _params->integrationRegion = set.drawIntegrationRegion->isChecked();
    _peakPxColor = set.colorIntPeak->color();
    _bkgPxColor = set.colorIntBkg->color();
    _peakPxColor.setAlphaF(set.alphaIntegrationRegion->value());
    _bkgPxColor.setAlphaF(set.alphaIntegrationRegion->value());
}

QVector<PeakItemGraphic*> PeakCollectionGraphics::peakItemGraphics(std::size_t frame_idx)
{
    if (!_peak_model)
        return {};

    if (!_peak_model->root())
        return {};

    if (_peak_model->root()->childCount() == 0)
        return {};

    std::vector<PeakItem*> peak_items = _peak_model->root()->peakItems();

    QVector<PeakItemGraphic*> graphics;
    for (PeakItem* peak_item : peak_items) {
        ohkl::Ellipsoid peak_ellipsoid = peak_item->peak()->shape();
        peak_ellipsoid.scale(peak_item->peak()->peakEnd());
        const ohkl::AABB& aabb = peak_ellipsoid.aabb();
        Eigen::Vector3d lower = aabb.lower();
        Eigen::Vector3d upper = aabb.upper();

        // If the current frame of the scene is out of the peak bounds do not paint it
        if (frame_idx < lower[2] || frame_idx > upper[2])
            continue;

        PeakItemGraphic* peak_graphic = peak_item->peakGraphic();
        peak_graphic->setCenter(frame_idx);
        peak_graphic->showLabel(_params->labels);
        peak_graphic->initFromPeakViewWidget(
            visualType(peak_item->peak()) ? _peak_view_widget->set1 : _peak_view_widget->set2);
        graphics.push_back(peak_graphic);
    }
    return graphics;
}

QVector<PeakCenterGraphic*> PeakCollectionGraphics::extPeakGraphics(std::size_t frame_idx)
{
    if (!_peak_center_data)
        return {};

    QVector<PeakCenterGraphic*> graphics;
    ohkl::XFileHandler* xfh = _peak_center_data->getFrame(frame_idx);

    if (!xfh)
        return graphics;

    for (const Eigen::Vector3d& vector : xfh->getPeakCenters()) {
        PeakCenterGraphic* center = new PeakCenterGraphic(vector);
        center->setColor(_spot_color);
        center->setSize(_spot_size);
        graphics.push_back(center);
    }
    return graphics;
}

QVector<PeakCenterGraphic*> PeakCollectionGraphics::detectorSpots(std::size_t frame_idx)
{
    if (!_keypoint_collection)
        return {};

    if (!_keypoint_collection->hasPeaks(frame_idx))
        return {};

    QVector<PeakCenterGraphic*> graphics;

    for (const cv::KeyPoint& point : *(_keypoint_collection->frame(frame_idx))) {
        PeakCenterGraphic* center =
            new PeakCenterGraphic({point.pt.x, point.pt.y, static_cast<double>(frame_idx)});
        center->setColor(_spot_color);
        center->setSize(_spot_size);
        graphics.push_back(center);
    }
    return graphics;
}

void PeakCollectionGraphics::getIntegrationMask(Eigen::MatrixXi& mask, std::size_t frame_idx)
{
    std::vector<PeakItem*> peak_items = _peak_model->root()->peakItems();

    for (PeakItem* peak_item : peak_items) {
        ohkl::Peak3D* peak = peak_item->peak();
        double peak_end, bkg_begin, bkg_end;
        if (_preview_int_regions) {
            if (_int_params.region_type == ohkl::RegionType::VariableEllipsoid) {
                peak_end = _int_params.peak_end;
                bkg_begin = _int_params.bkg_begin;
                bkg_end = _int_params.bkg_end;
            } else {
                peak_end = _int_params.fixed_peak_end;
                bkg_begin = _int_params.fixed_bkg_begin;
                bkg_end = _int_params.fixed_bkg_end;
            }
        } else {
            peak_end = peak_item->peak()->peakEnd();
            bkg_begin = peak_item->peak()->bkgBegin();
            bkg_end = peak_item->peak()->bkgEnd();
        }
        ohkl::IntegrationRegion region(peak, peak_end, bkg_begin, bkg_end, _int_params.region_type);
        if (region.isValid())
            region.updateMask(mask, frame_idx);
    }
}

void PeakCollectionGraphics::getSinglePeakIntegrationMask(
    ohkl::Peak3D* peak, Eigen::MatrixXi& mask, std::size_t frame_idx)
{
    if (!peak)
        return;

    double peak_end, bkg_begin, bkg_end;
    if (_preview_int_regions) {
        if (_preview_int_regions) {
            if (_int_params.region_type == ohkl::RegionType::VariableEllipsoid) {
                peak_end = _int_params.peak_end;
                bkg_begin = _int_params.bkg_begin;
                bkg_end = _int_params.bkg_end;
            } else {
                peak_end = _int_params.fixed_peak_end;
                bkg_begin = _int_params.fixed_bkg_begin;
                bkg_end = _int_params.fixed_bkg_end;
            }
        } else {
            peak_end = peak->peakEnd();
            bkg_begin = peak->bkgBegin();
            bkg_end = peak->bkgEnd();
        }

        ohkl::IntegrationRegion region(peak, peak_end, bkg_begin, bkg_end, _int_params.region_type);
        if (region.isValid())
            region.updateMask(mask, frame_idx);
    }
}

bool PeakCollectionGraphics::visualType(ohkl::Peak3D* peak)
{
    switch (_visual_type) {
        case VisualisationType::Enabled:
            if (peak->enabled())
                return true;
            break;
        case VisualisationType::Filtered:
            if (peak->caughtByFilter())
                return true;
            break;
    }
    return false;
}

QImage* PeakCollectionGraphics::getIntegrationRegionImage(std::size_t frame_idx, ohkl::Peak3D* peak)
{
    if (!_params->integrationRegion)
        return nullptr;

    ohkl::sptrDataSet data = _peak_model->dataSet();
    Eigen::MatrixXi mask(data->nRows(), data->nCols());
    mask.setConstant(int(EventType::EXCLUDED));
    initIntRegionFromPeakWidget();
    if (_params->singlePeakIntRegion) {
        if (peak)
            getSinglePeakIntegrationMask(peak, mask, frame_idx);
    } else
        getIntegrationMask(mask, frame_idx);

    QImage* region_img = new QImage(mask.cols(), mask.rows(), QImage::Format_ARGB32);

    for (int c = 0; c < mask.cols(); ++c) {
        for (int r = 0; r < mask.rows(); ++r) {
            EventType ev = EventType(mask(r, c));
            QColor color;

            switch (ev) {
                case EventType::PEAK: color = _peakPxColor; break;
                case EventType::BACKGROUND: color = _bkgPxColor; break;
                default: color = Qt::transparent; break;
            }

            region_img->setPixelColor(QPoint(c, r), color);
        }
    }
    return region_img;
}
