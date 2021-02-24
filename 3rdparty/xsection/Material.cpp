//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/Material.cpp
//! @brief     Implements class Material
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "ChemicalFormulaParser.h"
#include "IsotopeDatabaseManager.h"
#include "Material.h"
#include "Units.h"

namespace xsection {

Material::Material(const std::string& formula) : _formula(formula), _isotopes(), _massDensity(1.0)
{
    ChemicalFormulaParser<std::string::const_iterator> parser;
    bool success = boost::spirit::qi::phrase_parse(
        formula.begin(), formula.end(), parser, boost::spirit::qi::blank, _isotopes);
    if (!success)
        throw std::runtime_error("Invalid input formula");
}

Material* Material::clone() const
{
    return new Material(*this);
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
    if (massDensity <= 0.0)
        throw std::runtime_error("Invalid mass density");
    _massDensity = massDensity;
}

double Material::molarMass() const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    double molarMass(0.0);

    for (const auto& p : _isotopes)
        molarMass += p.second * imgr->property<double>(p.first, "molar_mass");

    return molarMass;
}

isotopeContents Material::massFractions() const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    isotopeContents massFractions;

    double molarMass(0.0);
    for (const auto& p : _isotopes) {
        massFractions.insert(
            std::make_pair(p.first, p.second * imgr->property<double>(p.first, "molar_mass")));
        molarMass += massFractions[p.first];
    }

    for (auto& p : massFractions)
        p.second /= molarMass;

    return massFractions;
}

isotopeContents Material::atomicNumberDensity() const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    isotopeContents nAtomsPerVolume;

    for (const auto& p : massFractions()) {
        double massFraction(_massDensity * p.second);
        double molarFraction = massFraction / imgr->property<double>(p.first, "molar_mass");
        nAtomsPerVolume.insert(std::make_pair(p.first, avogadro * molarFraction));
    }

    return nAtomsPerVolume;
}

double Material::muIncoherent() const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    double scatteringMuFactor = 0.0;
    for (const auto& p : atomicNumberDensity())
        scatteringMuFactor += p.second * imgr->property<double>(p.first, "xs_incoherent");
    return scatteringMuFactor;
}

double Material::muAbsorption(double lambda) const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    double absorptionMuFactor = 0.0;
    for (const auto& p : atomicNumberDensity()) {
        double thermalWavelength(1.798 * ang);
        absorptionMuFactor += p.second * imgr->property<double>(p.first, "xs_absorption") * lambda
            / thermalWavelength;
    }
    return absorptionMuFactor;
}

void Material::print(std::ostream& os) const
{
    os << "Formula = " << _formula << std::endl;
    for (const auto& p : _isotopes)
        os << "Isotope " << p.first << " --- " << p.second << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Material& material)
{
    material.print(os);
    return os;
}

} // namespace xsection
