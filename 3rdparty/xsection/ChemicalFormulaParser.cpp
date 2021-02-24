//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/ChemicalFormulaParser.cpp
//! @brief     Implements structs BuildElementFromUniqueIsotope, BuildElementFromNaturalIsotopes, ValidateIsotopeContents, BuildElementFromIsotopeMixture, BuildCompoundFromElement, ValidateCompoundContents, BuildMaterialFromCompounds
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "ChemicalFormulaParser.h"

bool xsection::BuildElementFromUniqueIsotope::operator()(
    isotopeContents& output, const std::string& isotopeName) const
{
    output.insert(std::make_pair(isotopeName, 1.0));
    return true;
}

bool xsection::BuildElementFromNaturalIsotopes::operator()(
    isotopeContents& output, const std::string& elementSymbol) const
{
    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();
    const auto& isotopeDatabase = imgr->isotopes();
    for (const auto& isotope : isotopeDatabase) {
        std::string symbolName = imgr->property<std::string>(isotope.first, "symbol");
        if (symbolName == elementSymbol) {
            output.insert(std::make_pair(
                              isotope.first, imgr->property<double>(
                                  isotope.first, "natural_abundance")));
        }
    }
    return true;
}

bool xsection::ValidateIsotopeContents::operator()(const isotopeContents& mixture) const
{
    double sumRatio(0.0);
    for (const auto& p : mixture) {
        if (p.second < 0.0 || p.second > 1.0)
            return false;
        sumRatio += p.second;
    }
    return (std::abs(1.0 - sumRatio) < 1.0e-6);
}

bool xsection::BuildElementFromIsotopeMixture::operator()(
        isotopeContents& output, const std::string& elementSymbol,
        const isotopeContents& mixture) const
{

    IsotopeDatabaseManager* imgr = IsotopeDatabaseManager::instance();

    double sumRatio(0.0);
    for (const auto& p : mixture) {
        if (p.second < 0.0 || p.second > 1.0)
            return false;
        std::string isotopeSymbol = imgr->property<std::string>(p.first, "symbol");
        if (isotopeSymbol.compare(elementSymbol) != 0)
            return false;
        sumRatio += p.second;
    }
    if (std::abs(1.0 - sumRatio) < 1.0e-6) {

        output = mixture;
        return true;
    } else {
        return false;
    }
}

bool xsection::BuildCompoundFromElement::operator()(
    isotopeContents& output, const isotopeContents& element, double stoichiometry) const
{
    if (stoichiometry <= 0.0)
        return false;

    for (auto isotope : element) {
        auto it = output.find(isotope.first);
        if (it == output.end())
            output.insert(std::make_pair(isotope.first, isotope.second * stoichiometry));
        else
            it->second += isotope.second * stoichiometry;
    }

    return true;
}

bool xsection::ValidateCompoundContents::operator()(const compoundList& mixture) const
{
    double sumRatio(0.0);
    for (const auto& p : mixture) {
        if (p.second < 0.0 || p.second > 1.0)
            return false;
        sumRatio += p.second;
    }
    return (std::abs(1.0 - sumRatio) < 1.0e-6);
}

bool xsection::BuildMaterialFromCompounds::operator()(
    isotopeContents& output, const compoundList& compounds) const
{

    for (const auto& compound : compounds) {
        for (const auto& isotope : compound.first) {
            auto it = output.find(isotope.first);
            double ratio = compound.second;
            double amount = ratio * isotope.second;
            if (it == output.end())
                output.insert(std::make_pair(isotope.first, amount));
            else
                it->second += amount;
        }
    }

    return (!output.empty());
}
