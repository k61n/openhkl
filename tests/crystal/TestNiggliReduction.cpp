#define BOOST_TEST_MODULE "Test Niggli Reduction"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <fstream>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/crystal/NiggliReduction.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Niggli_Reduction)
{

	const unsigned int nCells = 783;

	std::ifstream primitiveCellsFile("lattices.dat");
	std::ifstream niggliCellsFile("niggli_lattices.dat");

	std::string line;

	for (unsigned int i=0;i<4;++i) {
		std::getline(primitiveCellsFile,line);
		std::getline(niggliCellsFile,line);
	}

	std::istringstream iss;
	for (unsigned int i=0;i<nCells;++i) {
		std::getline(primitiveCellsFile,line);
		std::getline(niggliCellsFile,line);

		std::getline(primitiveCellsFile,line);
		iss.str(line);
		Eigen::Vector3d primitive_a;
		iss >> primitive_a[0] >> primitive_a[1] >> primitive_a[2];
		iss.clear();

		std::getline(niggliCellsFile,line);
		iss.str(line);
		Eigen::Vector3d niggli_a;
		iss >> niggli_a[0] >> niggli_a[1] >> niggli_a[2];
		iss.clear();

		std::getline(primitiveCellsFile,line);
		iss.str(line);
		Eigen::Vector3d primitive_b;
		iss >> primitive_b[0] >> primitive_b[1] >> primitive_b[2];
		iss.clear();

		std::getline(niggliCellsFile,line);
		iss.str(line);
		Eigen::Vector3d niggli_b;
		iss >> niggli_b[0] >> niggli_b[1] >> niggli_b[2];
		iss.clear();

		std::getline(primitiveCellsFile,line);
		iss.str(line);
		Eigen::Vector3d primitive_c;
		iss >> primitive_c[0] >> primitive_c[1] >> primitive_c[2];
		iss.clear();

		std::getline(niggliCellsFile,line);
		iss.str(line);
		Eigen::Vector3d niggli_c;
		iss >> niggli_c[0] >> niggli_c[1] >> niggli_c[2];
		iss.clear();

		nsx::UnitCell uc(primitive_a,primitive_b,primitive_c);
	    nsx::NiggliReduction niggli_reducer(uc.getMetricTensor(),1.0e-5);
	    Eigen::Matrix3d newg, P;
	    niggli_reducer.reduce(newg, P);
		std::cout<<i<<std::endl;
		std::cout<<uc.getReciprocalAVector().transpose()<<std::endl;
	    uc.transform(P);

	    Eigen::Vector3d calc_niggli_a = uc.getAVector();
	    BOOST_CHECK_CLOSE(calc_niggli_a[0],niggli_a[0],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_a[1],niggli_a[1],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_a[2],niggli_a[2],tolerance);

	    Eigen::Vector3d calc_niggli_b = uc.getBVector();
	    BOOST_CHECK_CLOSE(calc_niggli_b[0],niggli_b[0],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_b[1],niggli_b[1],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_b[2],niggli_b[2],tolerance);

	    Eigen::Vector3d calc_niggli_c = uc.getCVector();
	    BOOST_CHECK_CLOSE(calc_niggli_c[0],niggli_c[0],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_c[1],niggli_c[1],tolerance);
		BOOST_CHECK_CLOSE(calc_niggli_c[2],niggli_c[2],tolerance);
	}

	primitiveCellsFile.close();
	niggliCellsFile.close();
}
