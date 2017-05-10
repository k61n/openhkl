#define BOOST_TEST_MODULE "Test UBMinimizer"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <nsxlib/instrument/Component.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/crystal/LatticeMinimizer.h>
#include <nsxlib/crystal/LatticeSolution.h>
#include <nsxlib/crystal/UBMinimizer.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/instrument/Monochromator.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/MinimizerGSL.h>

using SX::Crystal::UBSolution;
using SX::Crystal::UBMinimizer;
using SX::Crystal::LatticeMinimizer;
using SX::Crystal::LatticeSolution;
using SX::Crystal::Peak3D;
using SX::Instrument::DetectorEvent;
using SX::Instrument::FlatDetector;
using SX::Instrument::Gonio;
using SX::Instrument::RotAxis;
using SX::Instrument::Sample;
using SX::Instrument::ComponentState;
using SX::Instrument::Source;
using SX::Utils::MinimizerGSL;

using namespace SX::Units;
using SX::Instrument::FlatDetector;
using std::shared_ptr;

// const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_UBMinimizer)
{
    // Build D9 instrument
    shared_ptr<FlatDetector> D9(new FlatDetector("D9-detector"));
    D9->setDistance(488*mm);
    D9->setDimensions(64*mm,64*mm);
    D9->setNPixels(32,32);

    // Build a detector
    std::shared_ptr<Gonio> detectorGonio(new Gonio("Gamma"));
    detectorGonio->addRotation("Gamma",Vector3d(0,0,1),RotAxis::CW);
    detectorGonio->addTranslation("y-offset",Vector3d(0,1,0));
    detectorGonio->getAxis("y-offset")->setPhysical(false);
    D9->setGonio(detectorGonio);


    shared_ptr<Sample> sample(new Sample("sample"));
    std::shared_ptr<Gonio> sampleGonio(new Gonio("Busing Levy convention"));
    sampleGonio->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
    sampleGonio->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
    sampleGonio->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);
    sample->setGonio(sampleGonio);

    std::shared_ptr<Source> source(new Source);
    SX::Instrument::Monochromator mono("mono");
    mono.setWavelength(0.8380);
    source->addMonochromator(mono);
    source->setSelectedMonochromator(0);

    UBMinimizer minimizer;
    minimizer.setDetector(D9);
    minimizer.setSample(sample);
    minimizer.setSource(source);
    minimizer.refineParameter(9,false); // Source
    minimizer.refineParameter(11,false); // Detector y
    minimizer.refineParameter(14,false); // Detector phi

    // Open the RAFUB input file to get all informations about the collected peaks
    std::ifstream ifs("CsOsO_15K.raf", std::ifstream::in);

    // fail test if we can't open the file
    BOOST_CHECK( ifs.fail() == false );

    double h,k,l,px,py,gamma,omega,chi,phi;
    std::vector<Peak3D> _peaks;

    while (ifs.good()) {

        ifs>>h>>k>>l>>px>>py>>gamma>>omega>>chi>>phi;

        // Create a peak
        Peak3D peak;
        // set the source (needed for wavelength)
        peak.setSource(source);
        // Create the detector event matching that peak (the px and py are given in mm in the RAFUB input file)
        peak.setDetectorEvent(DetectorEvent(*D9, px/2,py/2,{gamma*deg}));

        Eigen::RowVector3d hkl;
        hkl << h,k,l;

        // Create a sample state
        peak.setSampleState(ComponentState(sample.get(), {omega*deg,chi*deg,phi*deg}));

        minimizer.addPeak(peak,hkl);
    }


    Eigen::Matrix3d M;//=Eigen::Matrix3d::Identity();
    M<<sqrt(2)/2.0,-sqrt(2)/2,0,
       sqrt(2)/2.0, sqrt(2)/2,0,
       0          ,          0,1;
    minimizer.setStartingUBMatrix(M);
    minimizer.setMinimizer(new MinimizerGSL());
    minimizer.run(1000);

    UBSolution solution=minimizer.getSolution();

    SX::Crystal::UnitCell uc=SX::Crystal::UnitCell::fromReciprocalVectors(solution._ub.row(0),solution._ub.row(1),solution._ub.row(2));
    uc.setReciprocalCovariance(solution._covub);

    std::cout<<"FROM UB"<<std::endl;
    std::cout<<uc.getBusingLevyB()<<std::endl;
    std::cout<<uc.getBusingLevyU()<<std::endl;
}
