#include <stdexcept>

#include "DataSet.h"
#include "FitParameters.h"
#include "Gonio.h"
#include "IDataReader.h"
#include "Logger.h"
#include "Minimizer.h"
#include "Sample.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
{
    return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape()
{
}

Sample::Sample(const Sample& other): Component(other), _sampleShape(other._sampleShape)
{
}

Sample::Sample(const std::string& name): Component(name), _sampleShape()
{
}

Sample::Sample(const YAML::Node& node): Component(node)
{
}

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample()
{
}

Sample& Sample::operator=(const Sample& other)
{
    if (this != &other) {
        Component::operator=(other);
        _sampleShape = other._sampleShape;
    }
    return *this;
}

void Sample::setShape(const ConvexHull& shape)
{
    _sampleShape = shape;
}

ConvexHull& Sample::shape()
{
    return _sampleShape;
}

std::pair<bool,std::vector<double>> Sample::fitGonioOffsets(const DataList& dataset, size_t n_iterations, double tolerance) const
{
    auto sample_gonio = gonio();
    size_t n_axes = sample_gonio->axes().size();
    std::vector<double> fitted_offsets(n_axes,0);

    // No data provided, return zero offsets
    if (dataset.empty()) {
        nsx::info()<<"No data provided, offsets set to zero";
        return std::make_pair(false,fitted_offsets);
    }

    size_t n_selected_states(0);
    for (auto data : dataset) {
        auto&& states = data->instrumentStates();
        for (auto state : states) {
            if (!state.refined) {
                continue;
            }
            ++n_selected_states;
        }
    }

    if (n_selected_states < n_axes) {
        nsx::info()<<"No or not enough refined states found in the dataset for a reliable fit, offsets set to zero";
        return std::make_pair(false,fitted_offsets);
    }

    std::vector<Eigen::Matrix3d> selected_orientations;
    selected_orientations.reserve(n_selected_states);

    std::vector<std::vector<double>> selected_states;
    selected_states.reserve(n_selected_states);

    for (auto data : dataset) {
        auto&& states = data->instrumentStates();
        auto&& sample_states = data->dataReader()->sampleStates();
        for (size_t i = 0; i < states.size(); ++i) {
            auto state = states[i];
            if (state.refined) {
                selected_orientations.push_back(state.sampleOrientationMatrix());
                selected_states.push_back(sample_states[i]);
            }

        }
    }

    // Lambda to compute residuals
    auto residuals = [sample_gonio, &fitted_offsets, selected_orientations, selected_states] (Eigen::VectorXd& f) -> int
    {
        int n_obs = f.size();
        // Just duplicate the 0-residual to reach a "sufficient" amount of data points
        for (int i = 0; i < n_obs; ++i) {
            std::vector<double> real_values(selected_states[i].size(),0.0);
            const auto& state = selected_states[i];
            std::transform(state.begin(),state.end(),fitted_offsets.begin(),real_values.begin(),std::plus<double>());
            Eigen::Matrix3d fitted_sample_orientation = sample_gonio->affineMatrix(real_values).rotation();
            f(i) = (fitted_sample_orientation - selected_orientations[i]).norm();
        }

        return 0;
    };

    // Pass by address the parameters to be fitted to the parameter store
    nsx::FitParameters parameters;
    for (auto& v : fitted_offsets) {
        parameters.addParameter(&v);
    }

    // Set the Minimizer with the parameters store and the size of the residual vector
    nsx::Minimizer minimizer;
    // Hack to do the fit with GSL for having enough data points
    minimizer.initialize(parameters,n_selected_states);
    minimizer.set_f(residuals);
    minimizer.setxTol(tolerance);
    minimizer.setfTol(tolerance);
    minimizer.setgTol(tolerance);

    auto success = minimizer.fit(n_iterations);

    if (!success) {
        nsx::error()<<"Failed to fit sample orientation offsets";
        return std::make_pair(false,fitted_offsets);
    }

    return std::make_pair(true,fitted_offsets);}

} // end namespace nsx
