#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "ChemicalFormulaParser.h"
#include "IsotopeDatabaseManager.h"
#include "Material.h"
#include "../utils/Units.h"

namespace nsx
{

namespace Chemistry
{

Material::Material(const std::string& formula)
: _formula(formula),
  _isotopes(),
  _massDensity(1.0)
{
    ChemicalFormulaParser<std::string::const_iterator> parser;
    bool success = qi::phrase_parse(formula.begin(),formula.end(),parser,qi::blank,_isotopes);
    if (!success) {
        throw std::runtime_error("Invalid input formula");
    }
}

const std::string& Material::formula() const
{
    return _formula;
}

const isotopeContents& Material::isotopes() const
{
    return _isotopes;
}

double Material::massDensity() const
{
    return _massDensity;
}

void Material::setMassDensity(double massDensity)
{
    if (massDensity<=0.0) {
        throw std::runtime_error("Invalid mass density");
    }
    _massDensity = massDensity;
}

double Material::molarMass() const
{
    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

    double molarMass(0.0);

    for (const auto& p : _isotopes) {
        const auto& isotope=imgr->getIsotope(p.first);
        molarMass += p.second*isotope.getProperty<double>("molar_mass");
    }

    return molarMass;
}

isotopeContents Material::massFractions() const
{
    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

    isotopeContents massFractions;

    double molarMass(0.0);
    for (const auto& p : _isotopes){
        const auto& isotope=imgr->getIsotope(p.first);
        massFractions.insert(std::make_pair(p.first,p.second*isotope.getProperty<double>("molar_mass")));
        molarMass += massFractions[p.first];
    }

    for (auto& p : massFractions) {
        p.second/=molarMass;
    }

    return massFractions;
}

isotopeContents Material::atomicNumberDensity() const
{
    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

    isotopeContents nAtomsPerVolume;

    for (const auto& p : massFractions()) {
        const auto& isotope=imgr->getIsotope(p.first);
        double massFraction(_massDensity*p.second);
        double molarFraction = massFraction/isotope.getProperty<double>("molar_mass");
        nAtomsPerVolume.insert(std::make_pair(p.first,Units::avogadro*molarFraction));
    }

    return nAtomsPerVolume;
}

double Material::muIncoherent() const
{
    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

    double scatteringMuFactor=0.0;
    for (const auto& p : atomicNumberDensity()) {
        const auto& isotope=imgr->getIsotope(p.first);
        scatteringMuFactor+=p.second*isotope.getProperty<double>("xs_incoherent");
    }
    return scatteringMuFactor;
}

double Material::muAbsorption(double lambda) const
{
    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

    double absorptionMuFactor=0.0;
    for (const auto& p : atomicNumberDensity()) {
        const auto& isotope=imgr->getIsotope(p.first);
        double thermalWavelength(1.798*Units::ang);
        absorptionMuFactor+=p.second*isotope.getProperty<double>("xs_absorption")*lambda/thermalWavelength;
    }
    return absorptionMuFactor;
}

void Material::print(std::ostream& os) const
{
    os<<"Formula = "<<_formula<<std::endl;
    for (const auto & p : _isotopes)
    {
        os<<"Isotope "<<p.first << " --- "<<p.second<<std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Material& material)
{
    material.print(os);
    return os;
}

} // end namespace Chemistry

} // end namespace nsx
