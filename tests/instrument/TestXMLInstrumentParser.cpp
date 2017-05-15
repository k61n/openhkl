#define BOOST_TEST_MODULE "Test XML Instrument Parser"
#define BOOST_TEST_DYN_LINK

#include <iostream>
#include <memory>
#include <string>

#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <nsxlib/instrument/DetectorFactory.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/utils/Units.h>

using boost::property_tree::ptree;

using namespace nsx::Instrument;
using nsx::Units::UnitsManager;

BOOST_AUTO_TEST_CASE(Tests_XML_Instrument_Parser)
{
#pragma warning "test not implemented!"
//	UnitsManager* um = UnitsManager::Instance();
//
//	Diffractometer* diff=Diffractometer::create();
//
//	ptree pt;
//
//	read_xml("Instrument.xml",pt);
//
//	ptree instrNode = pt.get_child("instrument");
//
//	// Set the name of the instrument
//	std::string instrName=instrNode.get<std::string>("<xmlattr>.name");
//	diff->setName(instrName);
//
//	// Create the detector instance
//	DetectorFactory* detFactory = DetectorFactory::Instance();
//	ptree detNode=instrNode.get_child("detector");
//	std::string detType=detNode.get<std::string>("<xmlattr>.type");
//	std::string detName = detNode.get<std::string>("name");
//	Detector* detector = detFactory->create(detType,detName);
//
//	std::string name=detNode.get<std::string>("name");
//	detector->setName(name);
//
//	// Take the value of the sample_distance node
//	ptree sdistNode=detNode.get_child("sample_distance");
//	double dist=sdistNode.get_value<double>();
//	dist *= um->get(sdistNode.get<std::string>("<xmlattr>.units","mm"));
//	detector->setDistance(dist);
//
//	int nPixelsX=detNode.get<int>("npixels_x");
//	int nPixelsY=detNode.get<int>("npixels_y");
//
//	detector->setNPixels(nPixelsX, nPixelsY);
//	diff->setDetector(detector);
//
//	ptree gonioNode=detNode.get_child("gonio");
//
//	std::shared_ptr<Gonio> gonio(new Gonio("my-gonio"));
//
//	BOOST_FOREACH(ptree::value_type &v,gonioNode)
//	{
//		if (v.first.compare("axis")==0)
//		{
//			std::string axisType=v.second.get<std::string>("<xmlattr>.type");
//
////			std::string axisName=v.second.get<std::string>("name");
////			double nx=axisName=v.second.get<double>("nx");
////			double ny=axisName=v.second.get<double>("ny");
////			double nz=axisName=v.second.get<double>("nz");
////			bool physical=static_cast<bool>(axisName=v.second.get<int>("physical"));
////			if (axisType.compare("rotation")==0)
////			{
////			}
////			else if (axisType.compare("translation")==0)
////			{
////
////			}
//		}
//	}

}
