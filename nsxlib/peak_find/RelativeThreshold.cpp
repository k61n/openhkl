#include <stdexcept>

#include <Eigen/Dense>

#include "DataSet.h"
#include "RelativeThreshold.h"

namespace nsx {

RelativeThreshold::RelativeThreshold()
: Threshold()
{
    _parameters["background_scale"] = 2.0;
    _parameters["intensity_scale"] = 5.0;
}

RelativeThreshold::RelativeThreshold(const std::map<std::string,double>& parameters)
: RelativeThreshold()
{
    auto it1 = parameters.find("background_scale");
    if (it1 == parameters.end()) {
        throw std::runtime_error("Missing input parameters for relative threshold: background_scale");
    }

    auto it2 = parameters.find("intensity_scale");
    if (it2 == parameters.end()) {
        throw std::runtime_error("Missing input parameters for relative threshold: intensity_scale");
    }

    _parameters["background_scale"] = parameters.at("background_scale");
    _parameters["intensity_scale"] = parameters.at("intensity_scale");
}

double RelativeThreshold::value(sptrDataSet dataset, int frame) const
{
    Eigen::MatrixXd real_frame = dataset->frame(frame).cast<double>();

    size_t nrows = real_frame.rows();
    size_t ncols = real_frame.cols();

    // First crudly estimates the background as the total intensity of the frame divided by the number of pixels
    double bg = real_frame.sum()/nrows/ncols;

    double n_background = 0.0;

    double peak_counts = 0.0;

    for (int i = 0; i < nrows; ++i) {
        for (int j = 0; j < ncols; ++j) {
            if (real_frame(i,j) > bg + _parameters.at("background_scale")*std::sqrt(bg)) {
                peak_counts += real_frame(i,j);
                continue;
            }
            ++n_background;
        }
    }

    // Better estimation by removing the "peak" total intensity of the frame and divided by the number of background pixel
    bg = (real_frame.sum() - peak_counts)/n_background;

    double threshold = bg + _parameters.at("intensity_scale")*std::sqrt(bg);

    return threshold;
}

const char* RelativeThreshold::name() const
{
    return "relative offset";
}

} // end namespace nsx
