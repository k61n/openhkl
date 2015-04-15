#define BOOST_TEST_MODULE "Test FFT indexing"
#define BOOST_TEST_DYN_LINK
#include "UnitCell.h"
#include "FFTIndexing.h"
#include <boost/test/unit_test.hpp>
#include "Units.h"
#include <Eigen/Dense>
#include <vector>

using namespace SX::Crystal;
using namespace SX::Units;
const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_FFT_Indexing)
{
	UnitCell cell(5,7,9,90*deg,90*deg,90*deg);
	std::vector<Eigen::Vector3d> _qs;
	for (int h=-4;h<=4;++h)
	{
		for (int k=-4;k<=4;++k)
			{
			for (int l=-4;l<=4;++l)
				{
					_qs.push_back(cell.toReciprocalStandard(Eigen::Vector3d(h,k,l)));
				}
			}
	}
	// Max unit cell 50
	FFTIndexing indexing(50.0);
	indexing.addVectors(_qs);
}
