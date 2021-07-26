//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/GonioFit.cpp
//! @brief     Defines functions fit...GonioOffset
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/GonioFit.h"

#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "core/data/DataSet.h"
#include "core/loader/IDataReader.h"
#include <QDebug>
#include <QtGlobal>

namespace nsx {

GonioFit fitSampleGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance)
{
    const size_t n_axes = gonio.nAxes();
    std::vector<double> fitted_offsets(n_axes, 0);

    // No data provided, return zero offsets
    if (dataset.empty()) {
        qInfo() << "No data provided, offsets set to zero";
        return {false, std::move(fitted_offsets), {}};
    }

    size_t n_selected_states(0);
    for (const auto& data : dataset) {
        for (const auto& state : data->instrumentStates()) {
            if (!state.refined)
                continue;
            ++n_selected_states;
        }
    }

    if (n_selected_states < n_axes) {
        qInfo() << "No or not enough refined states found in the dataset for a "
                   "reliable fit, offsets set to zero";
        return {false, std::move(fitted_offsets), {}};
    }

    std::vector<Eigen::Matrix3d> myorientations;
    myorientations.reserve(n_selected_states);

    std::vector<std::vector<double>> selected_states;
    selected_states.reserve(n_selected_states);

    for (const auto& data : dataset) {
        auto&& states = data->instrumentStates();
        auto&& sample_states = data->reader()->sampleStates();
        for (size_t i = 0; i < states.size(); ++i) {
            auto state = states[i];
            if (state.refined) {
                myorientations.push_back(state.sampleOrientationMatrix());
                selected_states.push_back(sample_states[i]);
            }
        }
    }

    std::vector<double> cost_function;
    cost_function.reserve(n_iterations);

    // Lambda to compute residuals
    auto residuals = [gonio, &fitted_offsets, myorientations, selected_states,
                      &cost_function](Eigen::VectorXd& f) -> int {
        int n_obs = f.size();
        // Just duplicate the 0-residual to reach a "sufficient" amount of data points
        for (int i = 0; i < n_obs; ++i) {
            std::vector<double> real_values(selected_states[i].size(), 0.0);
            const auto& state = selected_states[i];
            std::transform(
                state.begin(), state.end(), fitted_offsets.begin(), real_values.begin(),
                std::plus<double>());
            Eigen::Matrix3d fitted_sample_orientation = gonio.affineMatrix(real_values).rotation();
            f(i) = (fitted_sample_orientation - myorientations[i]).norm();
        }

        cost_function.push_back(0.5 * f.norm());

        return 0;
    };

    // Pass by address the parameters to be fitted to the parameter store
    nsx::FitParameters parameters;
    for (auto& v : fitted_offsets)
        parameters.addParameter(&v);

    // Sets the Minimizer with the parameters store and the size of the residual vector
    nsx::Minimizer minimizer;
    // Hack to do the fit with GSL for having enough data points
    minimizer.initialize(parameters, n_selected_states);
    minimizer.set_f(residuals);
    minimizer.setxTol(tolerance);
    minimizer.setfTol(tolerance);
    minimizer.setgTol(tolerance);

    const bool success = minimizer.fit(n_iterations);

    if (!success) {
        qWarning() << "Failed to fit sample orientation offsets";
        return {false, std::move(fitted_offsets), {}};
    }

    return {true, std::move(fitted_offsets), std::move(cost_function)};
}

GonioFit fitDetectorGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance)
{
    const size_t n_axes = gonio.nAxes();
    std::vector<double> fitted_offsets(n_axes, 0.0);

    // No data provided, return zero offsets
    if (dataset.empty()) {
        qInfo() << "No data provided, offsets set to zero";
        return {false, std::move(fitted_offsets), {}};
    }

    size_t n_selected_states = 0;
    for (const auto& data : dataset) {
        auto&& states = data->instrumentStates();
        for (const auto& state : states) {
            if (!state.refined)
                continue;
            ++n_selected_states;
        }
    }

    if (n_selected_states < n_axes) {
        qInfo() << "No or not enough refined states found in the dataset for a "
                   "reliable fit, offsets set to zero";
        return {false, std::move(fitted_offsets), {}};
    }

    std::vector<Eigen::RowVector3d> myorientations;

    std::vector<std::vector<double>> selected_states;
    selected_states.reserve(n_selected_states);

    for (const auto& data : dataset) {
        const auto& states = data->instrumentStates();
        for (size_t i = 0; i < states.size(); ++i) {
            const auto& state = states[i];
            if (state.refined)
                myorientations.push_back(state.ni.normalized());
        }
    }

    std::vector<double> cost_function;
    cost_function.reserve(n_iterations);

    // Lambda to compute residuals
    auto residuals = [gonio, &fitted_offsets, myorientations,
                      &cost_function](Eigen::VectorXd& f) -> int {
        int n_obs = f.size();

        Eigen::Matrix3d fitted_detector_orientation =
            gonio.affineMatrix(fitted_offsets).rotation().transpose();

        // Just duplicate the 0-residual to reach a "sufficient" amout of data points
        for (int i = 0; i < n_obs; ++i) {
            f(i) = std::abs(
                myorientations[i].dot(Eigen::RowVector3d(0, 1, 0) * fitted_detector_orientation)
                - 1.0);
        }

        cost_function.push_back(0.5 * f.norm());

        return 0;
    };

    // Pass by address the parameters to be fitted to the parameter store
    nsx::FitParameters parameters;
    for (double v : fitted_offsets)
        parameters.addParameter(&v);

    // Sets the Minimizer with the parameters store and the size of the residual vector
    nsx::Minimizer minimizer;
    // Hack to do the fit with GSL for having enough data points
    minimizer.initialize(parameters, n_selected_states);
    minimizer.set_f(residuals);
    minimizer.setxTol(tolerance);
    minimizer.setfTol(tolerance);
    minimizer.setgTol(tolerance);

    const bool success = minimizer.fit(n_iterations);

    if (!success) {
        qWarning() << "Failed to fit detector orientation offsets";
        return {false, std::move(fitted_offsets), {}};
    }

    return {true, std::move(fitted_offsets), std::move(cost_function)};
}

} // namespace nsx
