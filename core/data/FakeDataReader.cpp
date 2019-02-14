#include <memory>

#include "blosc.h"

#include "BloscFilter.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "EigenToVector.h"
#include "FakeDataReader.h"
#include "Gonio.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace nsx {

FakeDataReader::FakeDataReader(const std::string &filename,
                               Diffractometer *diffractometer)
    : HDF5MetaDataReader(filename, diffractometer) {
  _frames.resize(_nFrames);

  for (size_t i = 0; i < _nFrames; ++i) {
    _frames[i].resize(_nRows, _nCols);
  }
}

Eigen::MatrixXi FakeDataReader::data(size_t frame) { return _frames[frame]; }

void FakeDataReader::setData(size_t frame, const Eigen::MatrixXi &data) {
  _frames[frame] = data;
}

} // end namespace nsx
