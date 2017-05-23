#include <limits>
#include <stdexcept>

#include <Eigen/Dense>

#include "../instrument/DetectorFactory.h"
#include "../instrument/Gonio.h"
#include "../instrument/MonoDetector.h"
#include "../instrument/MultiDetector.h"

namespace nsx {

Detector* MultiDetector::create(const boost::property_tree::ptree& node)
{
    return new MultiDetector(node);
}

MultiDetector::MultiDetector() : Composite<Detector,const boost::property_tree::ptree&>()
{
}

MultiDetector::MultiDetector(const MultiDetector& other) : Composite<Detector,const boost::property_tree::ptree&>(other)
{

}

MultiDetector::MultiDetector(const std::string& name) : Composite<Detector,const boost::property_tree::ptree&>()
{
    _name = name;
}

MultiDetector::MultiDetector(const boost::property_tree::ptree& node) : Composite<Detector,const boost::property_tree::ptree&>(node)
{
    // Set each subdetector of the multi detector from the property tree "detector" subnodes
    for (const auto& v : node)
    {
        if (v.first.compare("detector")==0)
        {
            // Fetch the detector from the factory
            MonoDetector* detector = dynamic_cast<MonoDetector*>(Detector::create(v.second));

            if (!detector)
                throw std::runtime_error("NSXTool does not support nested multi detector.");

            const auto& pixelOriginNode=v.second.get_child("origin");
            double opx=pixelOriginNode.get<double>("pixel_x");
            double opy=pixelOriginNode.get<double>("pixel_y");
            detector->setOrigin(opx,opy);
            add(detector);
        }
    }
}

MultiDetector::~MultiDetector()
{
}

Detector* MultiDetector::clone() const
{
    return new MultiDetector(*this);
}

unsigned int MultiDetector::getNPixels() const
{
    return getNCols()*getNRows();
}

unsigned int MultiDetector::getNCols() const
{
    return (getMaxCol()-getMinCol());
}

unsigned int MultiDetector::getNRows() const
{
    return (getMaxRow()-getMinRow());
}

int MultiDetector::getMinRow() const
{
    unsigned int minrow=std::numeric_limits<int>::infinity();
    for (auto& detector : _components)
    {
        unsigned int row= detector->getMinRow();
        if (row<minrow)
            minrow=row;
    }
    return minrow;
}

int MultiDetector::getMaxRow() const
{
    unsigned int maxrow=0;
    for (auto& detector : _components)
    {
        unsigned int row= detector->getMaxRow();
        if (row>maxrow)
            maxrow=row;
    }
    return maxrow;
}

int MultiDetector::getMinCol() const
{
    unsigned int mincol=std::numeric_limits<int>::infinity();
    for (auto& detector : _components)
    {
        unsigned int row= detector->getMinCol();
        if (row<mincol)
            mincol=row;
    }
    return mincol;
}

int MultiDetector::getMaxCol() const
{
    unsigned int maxcol=0;
    for (auto& detector : _components)
    {
        unsigned int row= detector->getMaxCol();
        if (row>maxcol)
            maxcol=row;
    }
    return maxcol;
}

bool MultiDetector::hasPixel(double px, double py) const
{
    for (auto& detector : _components)
        if (detector->hasPixel(px,py))
            return true;
    return false;
}

double MultiDetector::getHeight() const
{
    double height=0.0;
    for (auto& detector : _components)
        height += detector->getHeight();
    return height;
}

double MultiDetector::getWidth() const
{
    double width=0.0;
    for (auto& detector : _components)
        width += detector->getWidth();
    return width;
}

double MultiDetector::getAngularHeight() const
{
    double angularHeigth=0.0;
    for (auto& detector : _components)
        angularHeigth += detector->getAngularHeight();
    return angularHeigth;
}

double MultiDetector::getAngularWidth() const
{
    double angularWidth=0.0;
    for (auto& detector : _components)
        angularWidth += detector->getAngularWidth();
    return angularWidth;
}

Eigen::Vector3d MultiDetector::getPos(double px, double py) const
{

    for (auto& detector : _components)
    {
        if (detector->hasPixel(px,py))
        {
            if (!detector->hasGonio())
                return detector->getPos(px,py);
            else
                return detector->getGonio()->transform(detector->getPos(px,py));
        }

    }
    throw std::runtime_error("Detector: invalid pixel");
}

bool MultiDetector::hasKf(const Eigen::Vector3d& kf,const Eigen::Vector3d& from, double& px, double& py, double& t) const
{
    for (auto& detector : _components)
    {
        if (detector->hasKf(kf,from, px,py, t))
            return true;
    }
    return false;
}

unsigned int MultiDetector::getNDetectors() const
{
    return getNComponents();
}

} // end namespace nsx
