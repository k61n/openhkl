#define BOOST_TEST_MODULE "Test UBMinimizer"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include "Component.h"
#include "ComponentState.h"
#include "FlatDetector.h"
#include "Gonio.h"
#include "Peak3D.h"
#include "Sample.h"
#include "UBMinimizer.h"
#include "Units.h"
#include "UnitCell.h"

using SX::Crystal::UBSolution;
using SX::Crystal::UBMinimizer;
using SX::Crystal::Peak3D;
using SX::Instrument::DetectorEvent;
using SX::Instrument::FlatDetector;
using SX::Instrument::Gonio;
using SX::Instrument::RotAxis;
using SX::Instrument::Sample;
using SX::Instrument::ComponentState;

using namespace SX::Units;
const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_UBMinimizer)
{
	// Build D9 instrument
	SX::Instrument::Detector* D9=new FlatDetector("D9-detector");
	D9->setDistance(488*mm);
	D9->setDimensions(64*mm,64*mm);
	D9->setNPixels(32,32);

	// Build a detector
	std::shared_ptr<Gonio> detectorGonio(new Gonio("Gamma"));
	detectorGonio->addRotation("Gamma",Vector3d(0,0,1),RotAxis::CW);
	D9->setGonio(detectorGonio);

	Sample* sample=new Sample();
	std::shared_ptr<Gonio> sampleGonio(new Gonio("Busing Levy convention"));
	sampleGonio->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
	sampleGonio->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
	sampleGonio->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);
	sample->setGonio(sampleGonio);

	// Open the RAFUB input file to get all informations about the collected peaks
	std::ifstream ifs("CsOsO_15K.raf", std::ifstream::in);

	if (ifs.fail())
		return;

	double h,k,l,px,py,gamma,omega,chi,phi;
	std::vector<Peak3D> _peaks;

	while (ifs.good())
	{
		ifs>>h>>k>>l>>px>>py>>gamma>>omega>>chi>>phi;

		// Create a peak
		Peak3D peak;
		// Create the detector event matching that peak (the px and py are given in mm in the RAFUB input file)
		peak.setDetectorEvent(new DetectorEvent(D9->createDetectorEvent(px/2,py/2,{gamma*deg})));
		// set the miller indices corresponding to the peak
		peak.setMillerIndices(h,k,l);
		// Set the wavelength
		peak.setWavelength(0.8380);

		// Create a sample state
		peak.setSampleState(new ComponentState(sample->createState({omega*deg,chi*deg,phi*deg})));
		_peaks.push_back(peak);
	}

    UBMinimizer minimizer;
    minimizer.setDetector(D9);
    minimizer.setSample(sample);
    minimizer.setFixedParameters(9);
    minimizer.setFixedParameters(10);
    minimizer.setFixedParameters(11);
    minimizer.setFixedParameters(12);

    for (auto& peak : _peaks)
		minimizer.addPeak(peak);

    Eigen::Matrix3d M=Eigen::Matrix3d::Ones();
	minimizer.setStartingUBMatrix(M);

    int ret = minimizer.run(100);

    UBSolution solution=minimizer.getSolution();

//    std::cout<<solution<<std::endl;

    SX::Crystal::UnitCell uc=SX::Crystal::UnitCell::fromReciprocalVectors(solution._ub.row(0),solution._ub.row(1),solution._ub.row(2));

    uc.setReciprocalCovariance(solution._covub);

    std::cout<<uc<<std::endl;



}
