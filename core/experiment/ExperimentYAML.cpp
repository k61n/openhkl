//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ExperimentYAML.cpp
//! @brief     Implements class ExperimentYAML
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Ins titut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ExperimentYAML.h"

#include "core/algo/AutoIndexer.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/IIntegrator.h"
#include "core/loader/IDataReader.h"
#include "core/peak/IntegrationRegion.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/PeakMerger.h"


#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

#include <fstream>
#include <stdexcept>

namespace ohkl {

ExperimentYAML::ExperimentYAML(const std::string& filename)
{
    try {
        _node = YAML::LoadFile(filename);
        if (!_node["Experiment"])
            _node["Experiment"] = YAML::Null;

    } catch (const YAML::Exception& e) {
        _node["Experiment"] = YAML::Null;
    }
}

void ExperimentYAML::grabDataReaderParameters(DataReaderParameters* params) const
{
    ohklLog(Level::Info, "ExperimentYAML::grabDataReaderParameters: reading parameters from yml");
    if (!_node["DataReader"])
        throw std::runtime_error("No DataReader node in this yaml file");

    YAML::Node branch = _node["DataReader"];

    params->dataset_name = getNode<std::string>(branch, "name");
    params->wavelength = getNode<double>(branch, "wavelength");
    params->delta_chi = getNode<double>(branch, "delta_chi");
    params->delta_omega = getNode<double>(branch, "delta_omega");
    params->delta_phi = getNode<double>(branch, "delta_phi");
    params->bytes_per_pixel = getNode<int>(branch, "bytes_per_pixel");

    auto format = getNode<std::string>(branch, "format");
    if (format.empty())
        throw std::runtime_error("DataReader/format not specified");
    else {
        if (format == "raw")
            params->data_format = DataFormat::RAW;
        if (format == "tiff")
            params->data_format = DataFormat::TIFF;
        if (format == "text")
            params->data_format = DataFormat::PLAINTEXT;
    }

    if (format == "raw") {
        params->cols = getNode<int>(branch, "columns");
        params->rows = getNode<int>(branch, "rows");
        params->swap_endian = getNode<bool>(branch, "swap_endian");
        params->row_major = getNode<bool>(branch, "row_major");
    } else if (format == "tiff") {
        params->rebin_size = getNode<int>(branch, "rebin_size");
    }
}

void ExperimentYAML::setDataReaderParameters(DataReaderParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setDataReaderParameters: writing parameters to yml");
    YAML::Node reader_node;
    if (_node["DataReader"])
        reader_node = _node["DataReader"];
    else
        _node["DataReader"] = YAML::Null;

    reader_node["name"] = params->dataset_name;
    reader_node["wavelength"] = params->wavelength;
    reader_node["delta_chi"] = params->delta_chi;
    reader_node["delta_omega"] = params->delta_omega;
    reader_node["delta_phi"] = params->delta_phi;
    reader_node["bytes_per_pixel"] = params->delta_phi;
    if (params->data_format == DataFormat::RAW) {
        reader_node["format"] = "raw";
        reader_node["row_major"] = params->row_major;
        reader_node["swap_endian"] = params->swap_endian;
        reader_node["columns"] = params->cols;
        reader_node["rows"] = params->rows;
    } else if (params->data_format == DataFormat::TIFF) {
        reader_node["format"] = "tiff";
        reader_node["rebin_size"] = params->rebin_size;
    }
}

void ExperimentYAML::grabIntegrationParameters(IntegrationParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["Integration"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabIntegrationParameters: reading parameters from yml");
    YAML::Node branch = root["Integration"];

    params->peak_end = getNode<double>(branch, "peak_end");
    params->bkg_begin = getNode<double>(branch, "bkg_begin");
    params->bkg_end = getNode<double>(branch, "bkg_end");
    params->fixed_peak_end = getNode<double>(branch, "fixed_peak_end");
    params->fixed_bkg_begin = getNode<double>(branch, "fixed_bkg_begin");
    params->fixed_bkg_end = getNode<double>(branch, "fixed_bkg_end");
    params->fit_center = getNode<bool>(branch, "fit_center");
    params->fit_cov = getNode<bool>(branch, "fit_cov");
    params->integrator_type = static_cast<IntegratorType>(getNode<int>(branch, "integrator_type"));
    params->region_type = static_cast<RegionType>(getNode<int>(branch, "region_type"));
    params->gradient_type = static_cast<GradientKernel>(getNode<int>(branch, "gradient_type"));
    params->use_gradient = getNode<bool>(branch, "use_gradient");
    params->fft_gradient = getNode<bool>(branch, "fft_gradient");
    params->skip_masked = getNode<bool>(branch, "skip_masked");
    params->remove_overlaps = getNode<bool>(branch, "remove_overlaps");
    params->use_max_strength = getNode<bool>(branch, "use_max_strength");
    params->max_strength = getNode<double>(branch, "max_strength");
    params->use_max_d = getNode<bool>(branch, "use_max_d");
    params->max_d = getNode<double>(branch, "max_d");
    params->use_max_width = getNode<bool>(branch, "use_max_width");
    params->max_width = getNode<double>(branch, "max_width");
    params->discard_saturated = getNode<bool>(branch, "discard_saturated");
    params->max_counts = getNode<double>(branch, "max_counts");
}

void ExperimentYAML::setIntegrationParameters(IntegrationParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setIntegrationParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["Integration"])
        root["Integration"] = YAML::Null;
    YAML::Node int_node = root["Integration"];

    int_node["peak_end"] = params->peak_end;
    int_node["bkg_begin"] = params->bkg_begin;
    int_node["bkg_end"] = params->bkg_end;
    int_node["fixed_peak_end"] = params->fixed_peak_end;
    int_node["fixed_bkg_begin"] = params->fixed_bkg_begin;
    int_node["fixed_bkg_end"] = params->fixed_bkg_end;
    int_node["fit_center"] = params->fit_center;
    int_node["fit_cov"] = params->fit_cov;
    int_node["integrator_type"] = static_cast<int>(params->integrator_type);
    int_node["region_type"] = static_cast<int>(params->region_type);
    int_node["gradient_type"] = static_cast<int>(params->gradient_type);
    int_node["use_gradient"] = params->use_gradient;
    int_node["fft_gradient"] = params->fft_gradient;
    int_node["skip_masked"] = params->skip_masked;
    int_node["remove_overlaps"] = params->remove_overlaps;
    int_node["use_max_strength"] = params->use_max_strength;
    int_node["max_strength"] = params->max_strength;
    int_node["use_max_d"] = params->use_max_d;
    int_node["max_d"] = params->max_d;
    int_node["use_max_width"] = params->use_max_width;
    int_node["max_width"] = params->max_width;
    int_node["discard_saturated"] = params->discard_saturated;
    int_node["max_counts"] = params->max_counts;
}

void ExperimentYAML::grabPeakFinderParameters(PeakFinderParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["PeakFinder"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabPeakFinderParameters: reading parameters from yml");
    YAML::Node branch = root["PeakFinder"];

    params->minimum_size = getNode<int>(branch, "minimum_size");
    params->maximum_size = getNode<int>(branch, "maximum_size");
    params->peak_end = getNode<double>(branch, "peak_end");
    params->maximum_frames = getNode<int>(branch, "maximum_frames");
    params->frames_begin = getNode<int>(branch, "frames_begin");
    params->frames_end = getNode<int>(branch, "frames_end");
    params->threshold = getNode<double>(branch, "threshold");
    params->convolver = getNode<std::string>(branch, "convolver");
}

void ExperimentYAML::setPeakFinderParameters(PeakFinderParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setPeakFinderParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["PeakFinder"])
        root["PeakFinder"] = YAML::Null;
    YAML::Node pf_node = root["PeakFinder"];

    pf_node["minimum_size"] = params->minimum_size;
    pf_node["maximum_size"] = params->maximum_size;
    pf_node["peak_end"] = params->peak_end;
    pf_node["maximum_frames"] = params->maximum_frames;
    pf_node["frames_begin"] = params->frames_begin;
    pf_node["frames_end"] = params->frames_end;
    pf_node["threshold"] = params->threshold;
    pf_node["convolver"] = params->convolver;
}

void ExperimentYAML::grabAutoindexerParameters(IndexerParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["Autoindexer"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabAutoindexerParameters: reading parameters from yml");
    YAML::Node branch = root["Autoindexer"];

    params->maxdim = getNode<double>(branch, "maxdim");
    params->nSolutions = getNode<int>(branch, "nSolutions");
    params->nVertices = getNode<int>(branch, "nVertices");
    params->subdiv = getNode<int>(branch, "subdiv");
    params->indexingTolerance = getNode<double>(branch, "indexingTolerance");
    params->niggliTolerance = getNode<double>(branch, "niggliTolerance");
    params->gruberTolerance = getNode<double>(branch, "gruberTolerance");
    params->minUnitCellVolume = getNode<double>(branch, "minUnitCellVolume");
    params->unitCellEquivalenceTolerance = getNode<double>(branch, "unitCellEquivalenceTolerance");
    params->solutionCutoff = getNode<double>(branch, "solutionCutoff");
    params->frequencyTolerance = getNode<double>(branch, "frequencyTolerance");
    params->first_frame = getNode<int>(branch, "first_frame");
    params->last_frame = getNode<int>(branch, "last_frame");
    params->d_min = getNode<double>(branch, "d_min");
    params->d_max = getNode<double>(branch, "d_max");
    params->strength_min = getNode<double>(branch, "strength_min");
    params->strength_max = getNode<double>(branch, "strength_max");
    params->peaks_integrated = getNode<bool>(branch, "peaks_integrated");
}

void ExperimentYAML::setAutoindexerParameters(IndexerParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setAutoindexerParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["Autoindexer"])
        root["Autoindexer"] = YAML::Null;
    YAML::Node indexer_node = root["Autoindexer"];

    indexer_node["maxdim"] = params->maxdim;
    indexer_node["nSolutions"] = params->nSolutions;
    indexer_node["nVertices"] = params->nVertices;
    indexer_node["subdiv"] = params->subdiv;
    indexer_node["indexingTolerance"] = params->indexingTolerance;
    indexer_node["niggliTolerance"] = params->niggliTolerance;
    indexer_node["gruberTolerance"] = params->gruberTolerance;
    indexer_node["minUnitCellVolume"] = params->minUnitCellVolume;
    indexer_node["unitCellEquivalenceTolerance"] = params->unitCellEquivalenceTolerance;
    indexer_node["solutionCutoff"] = params->solutionCutoff;
    indexer_node["frequencyTolerance"] = params->frequencyTolerance;
    indexer_node["first_frame"] = params->first_frame;
    indexer_node["last_frame"] = params->last_frame;
    indexer_node["d_min"] = params->d_min;
    indexer_node["d_max"] = params->d_max;
    indexer_node["strength_min"] = params->strength_min;
    indexer_node["strength_max"] = params->strength_max;
    indexer_node["peaks_integrated"] = params->peaks_integrated;
}

void ExperimentYAML::grabShapeParameters(ShapeModelParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["ShapeModel"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabModelParameters: reading parameters from yml");
    YAML::Node branch = root["ShapeModel"];

    params->d_min = getNode<double>(branch, "d_min");
    params->d_max = getNode<double>(branch, "d_max");
    params->strength_min = getNode<double>(branch, "strength_min");
    params->kabsch_coords = getNode<bool>(branch, "kabsch_coords");
    params->nbins_x = getNode<int>(branch, "nbins_x");
    params->nbins_y = getNode<int>(branch, "nbins_y");
    params->nbins_z = getNode<int>(branch, "nbins_z");
    params->n_subdiv = getNode<int>(branch, "n_subdiv");
    params->sigma_m = getNode<double>(branch, "sigma_m");
    params->sigma_d = getNode<double>(branch, "sigma_d");
    params->neighbour_range_pixels = getNode<int>(branch, "neighbour_range_pixels");
    params->neighbour_range_frames = getNode<int>(branch, "neighbour_range_frames");
    params->interpolation = static_cast<PeakInterpolation>(getNode<int>(branch, "interpolation"));
}

void ExperimentYAML::setShapeParameters(ShapeModelParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setShapeParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["ShapeModel"])
        root["ShapeModel"] = YAML::Null;
    YAML::Node shape_node = root["ShapeModel"];

    shape_node["d_min"] = params->d_min;
    shape_node["d_max"] = params->d_max;
    shape_node["strength_min"] = params->strength_min;
    shape_node["kabsch_coords"] = params->kabsch_coords;
    shape_node["nbins_x"] = params->nbins_x;
    shape_node["nbins_y"] = params->nbins_y;
    shape_node["nbins_z"] = params->nbins_z;
    shape_node["n_subdiv"] = params->n_subdiv;
    shape_node["sigma_m"] = params->sigma_m;
    shape_node["sigma_d"] = params->sigma_d;
    shape_node["neighbour_range_pixels"] = params->neighbour_range_pixels;
    shape_node["neighbour_range_frames"] = params->neighbour_range_frames;
    shape_node["interpolation"] = static_cast<int>(params->interpolation);
}

void ExperimentYAML::grabPredictorParameters(PredictionParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["Predictor"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabPredictorParameters: reading parameters from yml");
    YAML::Node branch = root["Predictor"];

    params->d_min = getNode<double>(branch, "d_min");
    params->d_max = getNode<double>(branch, "d_max");
}

void ExperimentYAML::setPredictorParameters(PredictionParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setPredictorParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["Predictor"])
        root["Predictor"] = YAML::Null;
    YAML::Node predict_node = root["Predictor"];

    predict_node["d_min"] = params->d_min;
    predict_node["d_max"] = params->d_max;
}

void ExperimentYAML::grabMergeParameters(MergeParameters* params)
{
    YAML::Node root = _node["Experiment"];
    if (!root["Merge"])
        return;

    ohklLog(Level::Info, "ExperimentYAML::grabMergeParameters: reading parameters from yml");
    YAML::Node branch = root["Merge"];

    params->d_min = getNode<double>(branch, "d_min");
    params->d_max = getNode<double>(branch, "d_max");
    params->frame_min = getNode<int>(branch, "frame_min");
    params->frame_max = getNode<int>(branch, "frame_max");
    params->n_shells = getNode<int>(branch, "n_shells");
    params->friedel = getNode<bool>(branch, "friedel");
    params->scale = getNode<double>(branch, "scale");
}

void ExperimentYAML::setMergeParameters(MergeParameters* params)
{
    ohklLog(Level::Info, "ExperimentYAML::setMergeParameters: writing parameters to yml");
    YAML::Node root = _node["Experiment"];
    if (!root["Merge"])
        root["Merge"] = YAML::Null;
    YAML::Node merge_node = root["Merge"];

    merge_node["d_min"] = params->d_min;
    merge_node["d_max"] = params->d_max;
    merge_node["frame_min"] = params->frame_min;
    merge_node["frame_max"] = params->frame_max;
    merge_node["n_shells"] = params->n_shells;
    merge_node["friedel"] = params->friedel;
    merge_node["scale"] = params->scale;
    merge_node["d_max"] = params->d_max;
}

void ExperimentYAML::writeFile(const std::string& filename)
{
    std::ofstream fout(filename.c_str());
    fout << _node;
    fout.close();
}

} // namespace ohkl
