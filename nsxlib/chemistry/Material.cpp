#include <functional>
#include <numeric>
#include <iostream>

#include "../kernel/Error.h"
#include "Element.h"
#include "Material.h"
#include "../utils/Units.h"

namespace SX
{

namespace Chemistry
{

std::map<std::string,BuildingMode> Material::stringToBuildingMode={
        {"mass_fraction",BuildingMode::MassFraction},
        {"molar_fraction",BuildingMode::MolarFraction},
        {"stoichiometry",BuildingMode::Stoichiometry},
        {"partial_pressure",BuildingMode::PartialPressure}
};

std::map<BuildingMode,std::string> Material::buildingModeToString={
        {BuildingMode::MassFraction,"mass_fraction"},
        {BuildingMode::MolarFraction,"molar_fraction"},
        {BuildingMode::Stoichiometry,"stoichiometry"},
        {BuildingMode::PartialPressure,"partial_pressure"}
};

Material* Material::create(const std::string& name, BuildingMode mode)
{
    return (new Material(name,mode));
}

Material::Material(const std::string& name)
: _name(name),
  _buildingMode(BuildingMode::MolarFraction),
  _massDensity(0.0),
  _temperature(0.0),
  _elements(),
  _materials()
{
}

Material::Material(const std::string& name, BuildingMode mode)
: _name(name),
  _buildingMode(mode),
  _massDensity(0.0),
  _temperature(0.0),
  _elements(),
  _materials()
{
}

Material::~Material()
{
}

bool Material::operator==(const Material& other) const
{

    elementsMap mf1=getMolarFractions();
    elementsMap mf2=other.getMolarFractions();

    return (mf1==mf2);
}

const std::string& Material::getName() const
{
    return _name;
}

BuildingMode Material::getBuildingMode() const
{
    return _buildingMode;
}

double Material::getTemperature() const
{
    return _temperature;
}

double Material::getMassDensity() const
{
    return _massDensity;
}

void Material::setMassDensity(double massDensity)
{
    if (_buildingMode==BuildingMode::PartialPressure)
        return;

    if (massDensity<0.0)
        throw SX::Kernel::Error<Material>("Invalid mass density.");

    _massDensity = massDensity;
}

void Material::setTemperature(double temperature)
{
    if (temperature<0.0)
        throw SX::Kernel::Error<Material>("Invalid temperature.");

    _temperature=temperature;

    if (_buildingMode==BuildingMode::PartialPressure)
    {
        double totalPressure = std::accumulate(std::begin(_elements),std::end(_elements),0.0,[](double previous, const std::pair<sptrElement,double>& p){return previous+p.second;});
        totalPressure += std::accumulate(std::begin(_materials),std::end(_materials),0.0,[](double previous, const std::pair<sptrMaterial,double>& p){return previous+p.second;});
        // The molar density (eta) is computed as eta=P/RT
        _massDensity=getMolarMass()*totalPressure/SX::Units::R/_temperature;
    }
}

void Material::addElement(sptrElement element, double amount)
{
    auto it=_elements.find(element);

    if (it==_elements.end())
        _elements.insert(elementsMap::value_type(element,amount));
    else
        it->second += amount;
}

void Material::addElement(const std::string& name, double amount)
{
    sptrElement el(new Element(name));

    addElement(el,amount);
}

void Material::addMaterial(sptrMaterial material, double amount)
{
    auto it=_materials.find(material);

    if (it==_materials.end())
        _materials.insert(materialsMap::value_type(material,amount));
    else
        it->second += amount;
}

double Material::getMolarMass() const
{
    double mass(0.0);

    if (_buildingMode==BuildingMode::Stoichiometry)
    {
        for (auto e : _elements)
            mass += e.second*e.first->getMolarMass();
        for (auto m : _materials)
            mass += m.second*m.first->getMolarMass();
    }
    else
    {
        auto molarFractions=getMolarFractions();

        for (const auto& mf : molarFractions)
            mass += mf.second*mf.first->getMolarMass();
    }

    return mass;
}

elementsMap Material::getMassFractions() const
{

    elementsMap fractions;

    if (_buildingMode==BuildingMode::MassFraction)
    {
        for (auto e : _elements)
        {
            auto it=fractions.find(e.first);
            if (it==fractions.end())
                fractions.insert(elementsMap::value_type(e.first,e.second));
            else
                it->second += e.second;
        }
        for (auto m : _materials)
        {
            auto subfractions = m.first->getMassFractions();
            for (auto f : subfractions)
            {
                auto it=fractions.find(f.first);
                if (it==fractions.end())
                    fractions.insert(elementsMap::value_type(f.first,m.second*f.second));
                else
                    it->second += m.second*f.second;
            }
        }
    }
    else
    {
        auto molarFractions=getMolarFractions();
        double fact=0.0;
        for (auto mf : molarFractions)
        {
            double temp=mf.second*mf.first->getMolarMass();
            fact+=temp;
            fractions.insert(elementsMap::value_type(mf.first,temp));
        }
        for (auto& f : fractions)
            f.second/=fact;
    }

    return fractions;
}

elementsMap Material::getMolarFractions() const
{

    elementsMap fractions;

    if (_buildingMode==BuildingMode::MolarFraction)
    {
        for (auto e : _elements)
        {
            auto it=fractions.find(e.first);
            if (it==fractions.end())
                fractions.insert(elementsMap::value_type(e.first,e.second));
            else
                it->second += e.second;
        }
        for (auto m : _materials)
        {
            auto subfractions = m.first->getMolarFractions();
            for (auto f : subfractions)
            {
                auto it=fractions.find(f.first);
                if (it==fractions.end())
                    fractions.insert(elementsMap::value_type(f.first,m.second*f.second));
                else
                    it->second += m.second*f.second;
            }
        }
    }
    else if (_buildingMode==BuildingMode::PartialPressure || _buildingMode==BuildingMode::Stoichiometry)
    {
        double total = std::accumulate(std::begin(_elements),std::end(_elements),0.0,[](double previous, const std::pair<sptrElement,double>& p){return previous+p.second;});
        total += std::accumulate(std::begin(_materials),std::end(_materials),0.0,[](double previous, const std::pair<sptrMaterial,double>& p){return previous+p.second;});

        for (auto e : _elements)
        {
            auto it=fractions.find(e.first);
            if (it==fractions.end())
                fractions.insert(elementsMap::value_type(e.first,e.second/total));
            else
                it->second += e.second;
        }
        for (auto m : _materials)
        {
            auto subfractions = m.first->getMolarFractions();
            for (auto f : subfractions)
            {
                auto it=fractions.find(f.first);
                if (it==fractions.end())
                    fractions.insert(elementsMap::value_type(f.first,m.second*f.second/total));
                else
                    it->second += m.second*f.second;
            }
        }
    }
    else
    {
        auto massFractions=getMassFractions();
        double fact=0.0;
        for (auto mf : massFractions)
        {
            double temp=mf.second/mf.first->getMolarMass();
            fact+=temp;
            fractions.insert(elementsMap::value_type(mf.first,temp));
        }
        for (auto& f : fractions)
            f.second/=fact;
    }

    return fractions;
}

elementsMap Material::getNAtomsPerVolume() const
{
    elementsMap nAtomsPerVolume;

    double fact=Units::avogadro*_massDensity;

    auto massFractions=getMassFractions();

    for (const auto& mf : massFractions)
        nAtomsPerVolume.insert(elementsMap::value_type(mf.first,fact*mf.second/mf.first->getMolarMass()));

    return nAtomsPerVolume;
}

elementsMap Material::getNElectronsPerVolume() const
{
    elementsMap nElectronsPerVolume;

    double fact=Units::avogadro*_massDensity;

    for (const auto& mf : getMassFractions())
    {
        double nElectrons=static_cast<double>(mf.first->getNElectrons());
        nElectronsPerVolume.insert(elementsMap::value_type(mf.first,fact*nElectrons*mf.second/mf.first->getMolarMass()));
    }

    return nElectronsPerVolume;
}

double Material::getNAtomsTotalPerVolume() const
{
    auto nAtomsPerVolume=getNAtomsPerVolume();

    double nAtomsPerVolumeTotal = std::accumulate(std::begin(nAtomsPerVolume),
                                                   std::end(nAtomsPerVolume),
                                                   0.0,
                                                   [](double previous, const std::pair<sptrElement,double>& p){return previous+p.second;});
    return nAtomsPerVolumeTotal;
}

double Material::getNElectronsTotalPerVolume() const
{
    auto nElectronsPerVolume=getNElectronsPerVolume();

    double nElectronsPerVolumeTotal=std::accumulate(std::begin(nElectronsPerVolume),
                                                     std::end(nElectronsPerVolume),
                                                     0.0,
                                                     [](double previous, const std::pair<sptrElement,double>& p){return previous+p.second;});
    return nElectronsPerVolumeTotal;
}

double Material::getMuScattering() const
{
    double muScat=0.0;
    auto nAtomsPerVolume=getNAtomsPerVolume();
    for (const auto& p : nAtomsPerVolume)
    {
        double xsInc=p.first->getIncoherentXs();
        muScat+=p.second*xsInc;
    }
    return muScat;
}

double Material::getMuAbsorption(double lambda) const
{
    double muAbs=0.0;
    auto nAtomsPerVolume=getNAtomsPerVolume();
    for (const auto& p : nAtomsPerVolume)
    {
        double xsAbs=p.first->getAbsorptionXs(lambda);
        muAbs+=p.second*xsAbs;
    }
    return muAbs;
}

double Material::getMu(double lambda) const
{
    double mu=getMuScattering() + getMuAbsorption(lambda);
    return mu;
}

property_tree::ptree Material::writeToXML() const
{

    property_tree::ptree node;
    node.put("<xmlattr>.name",_name);
    node.put("<xmlattr>.building_mode",buildingModeToString[_buildingMode]);

    for (const auto& e : _elements)
    {
        auto enode=e.first->writeToXML();
        enode.put<double>("amount",e.second);
        node.add_child("element",enode);
    }

    for (const auto& m : _materials)
    {
        auto mnode=m.first->writeToXML();
        mnode.put<double>("amount",m.second);
        node.add_child("material",mnode);
    }

    return node;
}

void Material::print(std::ostream& os) const
{
    os<<"Name          = "<<_name<<std::endl;
    os<<"Building mode = "+buildingModeToString.at(_buildingMode)<<std::endl;
    for (const auto & m : _materials)
    {
        os<<"##########################################"<<std::endl;
        os<<"Submaterial"<<std::endl;
        os<<"##########################################"<<std::endl;
        os<<"amout         = "<<m.second<<std::endl;
        os<<*(m.first)<<std::endl;
    }
    for (const auto & e : _elements)
    {
        os<<"##########################################"<<std::endl;
        os<<"Element"<<std::endl;
        os<<"##########################################"<<std::endl;
        os<<"amout   = "<<e.second<<std::endl;
        os<<*(e.first);
    }
}

std::ostream& operator<<(std::ostream& os, const Material& material)
{
    material.print(os);
    return os;
}

} // end namespace Chemistry

} // end namespace SX
