#include <stdexcept>

#include <Eigen/Dense>

#include "DataSet.h"
#include "RelativeThreshold.h"

namespace nsx {

RelativeThreshold::RelativeThreshold()
: Threshold()
{
    _parameters["n_background_iterations"] = 1;
    _parameters["background_scale"] = 2.0;
    _parameters["intensity_scale"] = 5.0;
}

RelativeThreshold::RelativeThreshold(const std::map<std::string,double>& parameters)
: RelativeThreshold()
{
    setParameters(parameters);
}

double RelativeThreshold::value(sptrDataSet dataset, int frame) const
{
    Eigen::MatrixXd real_frame = dataset->frame(frame).cast<double>();

    size_t nrows = real_frame.rows();
    size_t ncols = real_frame.cols();

    const int n_iterations = static_cast<int>(_parameters.at("n_background_iterations"));
    const double background_scale = _parameters.at("n_background_iterations");

    // Estimates the background as the total intensity of the frame divided by the number of pixels
    double avg_bg(real_frame.sum()/static_cast<double>(nrows*ncols));

    for (int i = 0; i < n_iterations; ++i) {

        double sum_bg(0.0);
        double n_background(0.0);

        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j) {
                if (real_frame(i,j) < avg_bg + background_scale*std::sqrt(avg_bg)) {
                    sum_bg += real_frame(i,j);
                    ++n_background;
                }
            }
        }

        avg_bg = sum_bg/n_background;
    }

    double threshold = avg_bg + _parameters.at("intensity_scale")*std::sqrt(avg_bg);

    return threshold;
}

const char* RelativeThreshold::name() const
{
    return "relative offset";
}

} // end namespace nsx
