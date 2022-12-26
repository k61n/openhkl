//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DataSetGraphics.cpp
//! @brief     Implements class DataSetGraphics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/DataSetGraphics.h"

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/utils/Units.h"
#include "core/convolve/Convolver.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/detector/Detector.h"
#include "core/instrument/InterpolatedState.h"
#include "gui/graphics_items/BoxMaskItem.h"
#include "gui/graphics_items/EllipseMaskItem.h"
#include "gui/graphics_items/SXGraphicsItem.h"

#include <string>

#include <QPen>

DataSetGraphics::DataSetGraphics(DetectorSceneParams* params)
    : _data(nullptr)
    , _cell(nullptr)
    , _color_map(new ColorMap())
    , _params(params)
    , _beam_color(Qt::black)
    , _old_beam_color(Qt::gray)
    , _beam_size(20)
{
}

std::optional<QImage> DataSetGraphics::baseImage(std::size_t frame_idx, QRect full)
{
    if (!_data)
        return {};

    _params->currentIndex = frame_idx;
    _current_frame = _data->frame(frame_idx);
    if (_params->gradient) {
        return _color_map->matToImage(
            _data->gradientFrame(frame_idx, _params->gradientKernel, !_params->fftGradient)
                .cast<double>(),
            full, _params->intensity, _params->logarithmic);
    } else if (_params->filteredImage) {
        return _color_map->matToImage(filteredImage(_current_frame), full, 1);
    } else {
        return _color_map->matToImage(
            _current_frame.cast<double>(), full, _params->intensity, _params->logarithmic);
    }
}

Eigen::MatrixXd DataSetGraphics::filteredImage(RowMatrix image)
{
    int nrows = _data->nRows();
    int ncols = _data->nCols();

    Eigen::MatrixXd filtered_image = Eigen::MatrixXd::Zero(nrows, ncols);
    std::string convolver = ohkl::Convolver::kernelTypes.at(_params->convolver);
    _convolver.reset(ohkl::ConvolverFactory{}.create(convolver, _params->convolver_params));
    filtered_image = _convolver->convolve(image.cast<double>());

    for (int i = 0; i < nrows; ++i)
        for (int j = 0; j < ncols; ++j)
            filtered_image(i, j) = filtered_image(i, j) < _params->threshold ? 0 : 1;

    double minVal = filtered_image.minCoeff();
    double maxVal = filtered_image.maxCoeff();
    if (maxVal - minVal <= 0.0)
        maxVal = minVal + 1.0;
    filtered_image.array() -= minVal;
    filtered_image.array() /= maxVal - minVal;
    return filtered_image;
}

std::optional<QString> DataSetGraphics::tooltip(int col, int row)
{
    if (!_data)
        return {};

    const ohkl::Detector& det = _data->detector();
    const int nrows = int(det.nRows());
    const int ncols = int(det.nCols());
    if (col < 0 || col > ncols - 1 || row < 0 || row > nrows - 1)
        return {};

    switch (_params->tooltipMode) {
        case Pixel: return intensity(col, row);
        case GammaNu: return gammaNu(col, row);
        case Theta: return twoTheta(col, row);
        case DSpacing: return dSpacing(col, row);
        case MillerIndices: return millerIndices(col, row);
        default: return {};
    }
}

QVector<DirectBeamGraphic*> DataSetGraphics::beamGraphics(std::size_t frame_idx)
{
    QVector<DirectBeamGraphic*> graphics;
    for (auto&& event : *_beam) {
        double upper = double(frame_idx) + 0.01;
        double lower = double(frame_idx) - 0.01;
        if (event.frame < upper && event.frame > lower) {
            DirectBeamGraphic* beam = new DirectBeamGraphic();
            beam->setPos(event.px, event.py);
            beam->setZValue(10);
            beam->setAcceptHoverEvents(false);
            beam->setRect(-_beam_size / 2, -_beam_size / 2, _beam_size, _beam_size);
            QPen pen;
            pen.setCosmetic(true);
            pen.setColor(_beam_color);
            pen.setStyle(Qt::SolidLine);
            beam->setPen(pen);
            graphics.push_back(beam);
            break;
        }
    }

    for (auto&& event : *_old_beam) {
        double upper = double(_params->currentIndex) + 0.01;
        double lower = double(_params->currentIndex) - 0.01;
        if (event.frame < upper && event.frame > lower) {
            DirectBeamGraphic* beam = new DirectBeamGraphic();
            beam->setPos(event.px, event.py);
            beam->setZValue(10);
            beam->setAcceptHoverEvents(false);
            beam->setRect(-_beam_size / 2, -_beam_size / 2, _beam_size, _beam_size);
            QPen pen;
            pen.setCosmetic(true);
            pen.setColor(_old_beam_color);
            pen.setStyle(Qt::SolidLine);
            beam->setPen(pen);
            graphics.push_back(beam);
            break;
        }
    }
    return graphics;
}

QVector<MaskItem*> DataSetGraphics::maskGraphics()
{
    QVector<MaskItem*> graphics;
    for (auto* mask : _data->masks()) {
        if (dynamic_cast<const ohkl::BoxMask*>(mask) != nullptr) {
            BoxMaskItem* gmask = new BoxMaskItem(_data, new ohkl::AABB(mask->aabb()));
            gmask->setMask(dynamic_cast<ohkl::BoxMask*>(mask));
            gmask->setFrom(mask->aabb().lower());
            gmask->setTo(mask->aabb().upper());
            graphics.push_back(gmask);
        } else {
            EllipseMaskItem* gmask = new EllipseMaskItem(_data, new ohkl::AABB(mask->aabb()));
            gmask->setMask(dynamic_cast<ohkl::EllipseMask*>(mask));
            gmask->setFrom(mask->aabb().lower());
            gmask->setTo(mask->aabb().upper());
            graphics.push_back(gmask);
        }
    }
    return graphics;
}

int DataSetGraphics::pCount(int col, int row)
{
    return _current_frame(row, col);
}

std::optional<ohkl::InstrumentState> DataSetGraphics::instrumentState()
{
    ohkl::InstrumentState state;
    try {
        state =
            ohkl::InterpolatedState::interpolate(_data->instrumentStates(), _params->currentIndex);
    } catch (std::range_error& e) { // interpolation error for last frame
        return {};
    }
    return state;
}

std::optional<QString> DataSetGraphics::intensity(int col, int row)
{
    return QString("(%1,%2) I:%3").arg(col).arg(row).arg(pCount(col, row));
}

std::optional<QString> DataSetGraphics::gammaNu(int col, int row)
{
    auto state = instrumentState();
    if (!state)
        return {};

    ohkl::DirectVector pos = _data->detector().pixelPosition(col, row);
    double gamma = state.value().gamma(pos);
    double nu = state.value().nu(pos);
    return QString("(%1,%2) I: %3")
        .arg(gamma / ohkl::deg, 0, 'f', 3)
        .arg(nu / ohkl::deg, 0, 'f', 3)
        .arg(pCount(col, row));
}

std::optional<QString> DataSetGraphics::twoTheta(int col, int row)
{
    auto state = instrumentState();
    if (!state)
        return {};

    ohkl::DirectVector pos = _data->detector().pixelPosition(col, row);
    double th2 = state.value().twoTheta(pos);
    return QString("(%1) I: %2").arg(th2 / ohkl::deg, 0, 'f', 3).arg(pCount(col, row));
}

std::optional<QString> DataSetGraphics::dSpacing(int col, int row)
{
    auto state = instrumentState();
    if (!state)
        return {};

    double wave = _data->wavelength();
    ohkl::DirectVector pos = _data->detector().pixelPosition(col, row);
    double th2 = state.value().twoTheta(pos);
    return QString("(%1) I: %2").arg(wave / (2 * sin(0.5 * th2)), 0, 'f', 3).arg(pCount(col, row));
}

std::optional<QString> DataSetGraphics::millerIndices(int col, int row)
{
    auto state = instrumentState();
    if (!state)
        return {};

    if (!_cell)
        return {"No unit cell selected"};

    ohkl::DirectVector pos = _data->detector().pixelPosition(col, row);
    ohkl::ReciprocalVector q = state.value().sampleQ(pos);
    ohkl::MillerIndex miller_indices(q, *_cell);
    Eigen::RowVector3d hkl = miller_indices.rowVector().cast<double>() + miller_indices.error();
    return QString("(%1,%2,%3) I: %4")
        .arg(hkl[0], 0, 'f', 2)
        .arg(hkl[1], 0, 'f', 2)
        .arg(hkl[2], 0, 'f', 2)
        .arg(pCount(col, row));
}
