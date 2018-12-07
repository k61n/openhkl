#include "ConvolvedFrame.h"
#include "ConvolverFactory.h"
#include "IDataReader.h"

namespace nsx {

Eigen::MatrixXd convolvedFrame(
    DataSet& dataSet, std::size_t idx,
    const std::string& convolver_type, const std::map<std::string,double>& parameters)
{
    ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create(convolver_type,parameters);
    const Eigen::MatrixXi frame_data = dataSet.reader()->data(idx);
    return convolver->convolve(frame_data.cast<double>());
}

}
