#include "GonioState.h"

namespace SX {
namespace Instrument {

GonioState::GonioState():_gonio(nullptr),
		_values(),
		_transformation(Eigen::Transform<double,3,Eigen::Affine>::Identity())
{
}
GonioState::~GonioState()
{
}


} // end namespace Instrument
} // End namespace SX
