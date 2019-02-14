#include <Eigen/Dense>

#include <core/NSXTest.h>
#include <core/ReciprocalVector.h>

NSX_INIT_TEST

int main() {

  Eigen::RowVector3d v1(1, 2, 3);

  nsx::ReciprocalVector rv1(v1);

  double &x = rv1[0];

  x = 100;

  NSX_CHECK_EQUAL(rv1[0], 100);

  return 0;
}
