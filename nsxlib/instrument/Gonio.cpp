#include <algorithm>
#include <stdexcept>

#include <Eigen/Geometry>

#include "../instrument/Gonio.h"
#include "../instrument/RotAxis.h"
#include "../instrument/TransAxis.h"
#include "../utils/Units.h"

namespace nsx {

Gonio::Gonio() : _label("goniometer")
{
}

Gonio::Gonio(const std::string& label) : _label(label)
{
}

Gonio::Gonio(const Gonio& other) : _label(other._label)
{
    _axes.reserve(other._axes.size());
    for (auto ax : other._axes)
        _axes.push_back(ax->clone());
}

Gonio::Gonio(const boost::property_tree::ptree& node)
{

    _label=node.get<std::string>("name","");

    // Set the axis of the detector goniometer from the XML node
    for(const auto& v : node)
    {
        if (v.first.compare("axis")==0)
            addAxis(Axis::create(v.second));
    }
}

Gonio& Gonio::operator=(const Gonio& other)
{
    if (this != &other)
    {
        _label = other._label;
        _axes.reserve(other._axes.size());
        for (auto ax : other._axes)
            _axes.push_back(ax->clone());
    }

    return *this;
}

Gonio::~Gonio()
{
    for (auto ax : _axes)
        delete ax;
}

const std::vector<Axis*>& Gonio::getAxes() const
{
    return _axes;
}

std::vector<unsigned int> Gonio::getPhysicalAxesIds() const
{
    std::vector<unsigned int> ids;
    ids.reserve(getNPhysicalAxes());
    for (auto a : _axes) {
        if (a->isPhysical())
            ids.push_back(a->getId());
    }
    return ids;
}

std::vector<std::string> Gonio::getPhysicalAxesNames() const
{
    std::vector<std::string> names;
    names.reserve(getNPhysicalAxes());
    for (auto a : _axes)
    {
        if (a->isPhysical())
            names.push_back(a->getLabel());
    }
    return names;
}

std::map<unsigned int,std::string> Gonio::getPhysicalAxisIdToNames() const
{
    std::map<unsigned int,std::string> idToNames;
    for (auto a : _axes)
    {
        if (a->isPhysical())
            idToNames.insert(std::map<unsigned int,std::string>::value_type(a->getId(),a->getLabel()));
    }
    return idToNames;
}

std::vector<std::string> Gonio::getAxesNames() const
{
    std::vector<std::string> names;
    names.reserve(getNAxes());
    for (auto a : _axes)
            names.push_back(a->getLabel());
    return names;
}

Axis* Gonio::getAxis(unsigned int i)
{
    isAxisValid(i);
    return _axes[i];
}

Axis* Gonio::getAxisFromId(unsigned int id)
{
    unsigned int i=isAxisIdValid(id);
    return _axes[i];
}

Axis* Gonio::getAxis(const std::string& label)
{
    unsigned int i=isAxisValid(label);
    return _axes[i];
}

void Gonio::addAxis(Axis* axis)
{
    _axes.push_back(axis);
}

Axis* Gonio::addRotation(const std::string& label, const Eigen::Vector3d& axis,RotAxis::Direction dir)
{
    _axes.push_back(new RotAxis(label,axis,dir));
    return _axes.back();
}

Axis* Gonio::addTranslation(const std::string& label, const Eigen::Vector3d& axis)
{
    _axes.push_back(new TransAxis(label,axis));
    return _axes.back();
}

void Gonio::isAxisValid(unsigned int i) const
{
    if (i>=_axes.size())
        throw std::invalid_argument("Trying to access non-valid axis");
}

unsigned int Gonio::isAxisValid(const std::string& label) const
{
    for (unsigned int i=0;i<_axes.size();++i)
    {
        if (_axes[i]->getLabel().compare(label)==0)
            return i;
    }
    //! If not found
    throw std::invalid_argument("Could not find the label "+label+" as a goniometer axis in "+_label);
}

unsigned int Gonio::isAxisIdValid(unsigned int id) const
{
    for (unsigned int i=0;i<_axes.size();++i)
    {
        if (_axes[i]->getId()==id)
            return i;
    }
    //! If not found
    throw std::invalid_argument("Could not find any axis with id "+std::to_string(id));
}

Eigen::Transform<double,3,Eigen::Affine> Gonio::getHomMatrix(const std::vector<double>& values) const
{
    if (values.size() != getNPhysicalAxes())
    {
        throw std::range_error("Trying to set Gonio "+_label+" with wrong number of parameters");
    }
    Eigen::Transform<double,3,Eigen::Affine> result=Eigen::Transform<double,3,Eigen::Affine>::Identity();
    std::vector<Axis*>::const_reverse_iterator it;
    std::vector<double>::const_reverse_iterator itv=values.rbegin();

    for (it=_axes.rbegin();it!=_axes.rend();++it)
    {
        if ((*it)->isPhysical())
        {
            result=(*it)->getHomMatrix(*itv)*result;
            itv++;
        }
        else
            result=(*it)->getHomMatrix(0.0)*result;
    }
    return result;
}

Eigen::Transform<double,3,Eigen::Affine> Gonio::getInverseHomMatrix(const std::vector<double>& values) const
{
    return getHomMatrix(values).inverse();
}

std::size_t Gonio::getNAxes() const
{
    return _axes.size();
}

std::size_t Gonio::getNPhysicalAxes() const
{
    std::size_t nPhysAxis = 0;
    for (auto a : _axes)
    {
        if (a->isPhysical())
            nPhysAxis++;
    }
    return nPhysAxis;
}

void Gonio::resetOffsets()
{
    for (unsigned int i=0;i<_axes.size();++i)
    {
        _axes[i]->setOffset(0.0);
    }
}

Eigen::Vector3d Gonio::transform(const Eigen::Vector3d& v,const std::vector<double>& values)
{
    Eigen::Transform<double,3,Eigen::Affine> result=getHomMatrix(values);
    return (result*v.homogeneous());
}

void Gonio::transformInPlace(Eigen::Vector3d& v,const std::vector<double>& values)
{
    Eigen::Transform<double,3,Eigen::Affine> result=getHomMatrix(values);
    v=result*v.homogeneous();
}

Eigen::Vector3d Gonio::transformInverse(const Eigen::Vector3d& v,const std::vector<double>& values)
{
    Eigen::Transform<double,3,Eigen::Affine> result=getInverseHomMatrix(values);
    return (result*v.homogeneous());
}

void Gonio::transformInverseInPlace(Eigen::Vector3d& v,const std::vector<double>& values)
{
    v=getInverseHomMatrix(values)*v.homogeneous();
}

bool Gonio::hasAxis(const std::string& name) const
{
    for (const auto& axis : _axes)
    {
        if (name.compare(axis->getLabel())==0)
            return true;
    }
    return false;
}

bool Gonio::hasPhysicalAxis(const std::string& name) const
{
    for (const auto& axis : _axes)
    {
        if (name.compare(axis->getLabel())==0 && axis->isPhysical())
            return true;
    }
    return false;
}

} // end namespace nsx

