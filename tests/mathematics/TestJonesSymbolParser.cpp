#include <string>
#include <vector>

#include <Eigen/Dense>

#include <core/JonesSymbolParser.h>
#include <core/NSXTest.h>

#include <iostream>

const double tolerance = 1e-6;

NSX_INIT_TEST

int main() {
  Eigen::Transform<double, 3, Eigen::Affine> matrix;

  NSX_CHECK_NO_THROW(
      matrix = nsx::parseJonesSymbol(" x+4y-z+1/2,-x + y -3z +2,-x-y-z +3\t"));

  // Compare
  NSX_CHECK_CLOSE(matrix(0, 0), 1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(0, 1), 4.0, tolerance);
  NSX_CHECK_CLOSE(matrix(0, 2), -1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(0, 3), 0.5, tolerance);
  NSX_CHECK_CLOSE(matrix(1, 0), -1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(1, 1), 1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(1, 2), -3.0, tolerance);
  NSX_CHECK_CLOSE(matrix(1, 3), 2.0, tolerance);
  NSX_CHECK_CLOSE(matrix(2, 0), -1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(2, 1), -1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(2, 2), -1.0, tolerance);
  NSX_CHECK_CLOSE(matrix(2, 3), 3.0, tolerance);
  NSX_CHECK_SMALL(matrix(3, 0), tolerance);
  NSX_CHECK_SMALL(matrix(3, 1), tolerance);
  NSX_CHECK_SMALL(matrix(3, 2), tolerance);
  NSX_CHECK_CLOSE(matrix(3, 3), 1.0, tolerance);

  NSX_CHECK_NO_THROW(matrix = nsx::parseJonesSymbol("x,y,z"));

  // Compare
  NSX_CHECK_CLOSE(matrix(0, 0), 1.0, tolerance);
  NSX_CHECK_SMALL(matrix(0, 1), tolerance);
  NSX_CHECK_SMALL(matrix(0, 2), tolerance);
  NSX_CHECK_SMALL(matrix(0, 3), tolerance);
  NSX_CHECK_SMALL(matrix(1, 0), tolerance);
  NSX_CHECK_CLOSE(matrix(1, 1), 1.0, tolerance);
  NSX_CHECK_SMALL(matrix(1, 2), tolerance);
  NSX_CHECK_SMALL(matrix(1, 3), tolerance);
  NSX_CHECK_SMALL(matrix(2, 0), tolerance);
  NSX_CHECK_SMALL(matrix(2, 1), tolerance);
  NSX_CHECK_CLOSE(matrix(2, 2), 1.0, tolerance);
  NSX_CHECK_SMALL(matrix(2, 3), tolerance);
  NSX_CHECK_SMALL(matrix(3, 0), tolerance);
  NSX_CHECK_SMALL(matrix(3, 1), tolerance);
  NSX_CHECK_SMALL(matrix(3, 2), tolerance);
  NSX_CHECK_CLOSE(matrix(3, 3), 1.0, tolerance);

  NSX_CHECK_THROW_ANY(matrix = nsx::parseJonesSymbol("2x"));

  NSX_CHECK_THROW_ANY(matrix = nsx::parseJonesSymbol("2x,4y"));

  NSX_CHECK_THROW_ANY(matrix = nsx::parseJonesSymbol("1,1,1"));

  NSX_CHECK_THROW_ANY(matrix = nsx::parseJonesSymbol("2x,ay+3z,$z+1/2"));

  return 0;
}
