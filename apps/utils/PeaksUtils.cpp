#include <limits>

#include <core/Peak3D.h>

#include "PeaksUtils.h"

std::pair<double, double> dRange(const nsx::PeakList &peaks) {
  std::pair<double, double> drange;
  double dmin = std::numeric_limits<double>::infinity();
  double dmax = -std::numeric_limits<double>::infinity();

  for (auto peak : peaks) {
    double d = 1.0 / peak->q().rowVector().norm();
    dmin = std::min(dmin, d);
    dmax = std::max(dmax, d);
  }

  return std::make_pair(dmin, dmax);
}
