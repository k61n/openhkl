/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_FORMULAPARSER_H_
#define NSXTOOL_FORMULAPARSER_H_

#define BOOST_SPIRIT_USE_PHOENIX_V3
//#define BOOST_RESULT_OF_USE_DECLTYPE

#include <map>
#include <string>
#include <vector>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/numeric.hpp>

#include "Isotope.h"
#include "IsotopeDatabaseManager.h"

namespace SX {

namespace Chemistry {

namespace qi = boost::spirit::qi;

//! map an isotope name to its ratio in a given compound
using isotopeContents = std::map<std::string,double>;

//! define a material as a list of compound, defined through their isotope contents, and
//! their associated molar ratio in the compound
using compoundList = std::vector<std::pair<isotopeContents,double>>;

//! @Class: BuildElementFromUniqueIsotope.
//! Functor for setting up a chemical element made of a unique isotope.
//! In such case, the isotope contents of the element is made of that sngle istope
//! and a molar ratio of 1.0
struct BuildElementFromUniqueIsotope
{
    bool operator()(isotopeContents &output, const std::string& isotopeName) const
    {
        output.insert(std::make_pair(isotopeName,1.0));

        return true;
    }
};

//! @Class: BuildElementFromNaturalIsotopes.
//! Functor for setting up a chemical element in its natural state.
//! In such case, the isotope contents of the element is made of the all
//! the isotopes that build the element with their natural abundances.
struct BuildElementFromNaturalIsotopes
{
	bool operator()(isotopeContents &output, const std::string& elementSymbol) const
	{
	    std::cout<<"rfjksdlfjkdljklfjs"<<std::endl;
	    IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();
		const auto& isotopeDatabase=imgr->getDatabase();
		for (const auto& isotope : isotopeDatabase) {
			std::string symbolName = isotope.second.getProperty<std::string>("symbol");
			if (symbolName == elementSymbol) {
				output.insert(std::make_pair(isotope.second.getName(),isotope.second.getProperty<double>("natural_abundance")));
			}
		}
		return true;
	}
};

//! @Class: ValidateIsotopeContents.
//! Functor for validating the sum of the molar ratio of a set of isotopes.
//! The sum of the ratio must be equal to 1 within a given tolerance
struct ValidateIsotopeContents {

    bool operator()(const isotopeContents& mixture) const {
        std::cout<<"fsdfsdf"<<std::endl;
        double sumRatio(0.0);
        for (const auto& p : mixture) {
            std::cout<<p.first<<p.second<<std::endl;
            if (p.second < 0.0 || p.second > 1.0) {
                return false;
            }
            sumRatio += p.second;
        }
        return (std::abs(1.0 - sumRatio) < 1.0e-6);
    }

};

//! @Class: BuildElementFromIsotopeMixture.
//! Functor for setting up a chemical element using a mixture of istopes.
//! In such case, the isotope contents of the element is made of the input isotopes
//! with their corresponding input molar ratio.
struct BuildElementFromIsotopeMixture {

	bool operator()(isotopeContents& output, const std::string& elementSymbol, const isotopeContents& mixture) const {

        IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();

		double sumRatio(0.0);
		for (const auto& p : mixture) {
			if (p.second < 0.0 || p.second > 1.0) {
				return false;
			}
			const auto& isotope = imgr->getIsotope(p.first);
			std::string isotopeSymbol = isotope.getProperty<std::string>("symbol");
			if (isotopeSymbol.compare(elementSymbol)!=0) {
				return false;
			}
			sumRatio += p.second;
		}
		if (std::abs(1.0 - sumRatio) < 1.0e-6) {

			output = mixture;
			return true;
		} else {
			return false;
		}
	}
};

//! @Class: BuildCompoundFromElement.
//! Functor for building incrementally a chemical compound from a chemical element.
//! The isotope contents of the chemical compound is updated with the isotope contents
//! of the chemical element multiplied by its stoichiometry
struct BuildCompoundFromElement
{
	bool operator()(isotopeContents &output, const isotopeContents& element, double stoichiometry) const
	{
	    std::cout<<"sfksdlfjksdljfksdjffjkdkd"<<std::endl;

		if (stoichiometry<=0.0)
			return false;

		for (auto isotope : element) {
			auto it = output.find(isotope.first);
			if (it == output.end()) {
				output.insert(std::make_pair(isotope.first,isotope.second*stoichiometry));
			} else {
				it->second += isotope.second*stoichiometry;
			}
		}

		return true;
	}
};

//! @Class: ValidateCompoundContents.
//! Functor for validating the sum of the molar ratios of a set of chemical compounds.
//! The sum of the ratio must be equal to 1 within a given tolerance
struct ValidateCompoundContents {

    bool operator()(const compoundList& mixture) const {
        double sumRatio(0.0);
        for (const auto& p : mixture) {
            if (p.second < 0.0 || p.second > 1.0) {
                return false;
            }
            sumRatio += p.second;
        }
        return (std::abs(1.0 - sumRatio) < 1.0e-6);
    }

};

//! @Class: BuidMaterialFromCompounds.
//! Functor for building material from a list of chemical compounds and their associated molar ratio in the material.
//! The isotope contents of the material is updated with the isotope contents
//! of the chemical compound multiplied by its molar ratio
struct BuidMaterialFromCompounds
{
	bool operator()(isotopeContents &output, const compoundList& compounds) const
	{

		for (const auto& compound : compounds) {
			for (const auto& isotope : compound.first) {
			    std::cout<<isotope.first<<std::endl;
				auto it = output.find(isotope.first);
				double ratio = compound.second;
				double amount = ratio*isotope.second;
				if (it == output.end()) {
					output.insert(std::make_pair(isotope.first,amount));
				} else {
					it->second += amount;
				}
			}
		}

		return true;
	}
};

//! @Class: ChemicalFormulaParser.
//! Parses a chemical formula.
//! The chemical formula is defined as a string that represents a sequence of elements (e.g. CH4, H2O, C[12]3H[2]8).
//! Each element is in turn defined by a string that represents its chemical symbol as defined in the periodic table respectively followed
//! optionally by the element isotope given in square brackets and by the number of such element in the chemical formula (1 if not provided).
//! The parser is used as usual in spirit given two iterators (begin and end) : parse(begin,end,parser)
//! Example: C3H[2]8 will parse a molecule with three natural  carbons and eight deuterium atoms i.e. a deuterated propane.
template <typename Iterator>
struct ChemicalFormulaParser : qi::grammar<Iterator,isotopeContents()>
{
    ChemicalFormulaParser(): ChemicalFormulaParser::base_type(_mixtureToken)
    {
        using namespace qi;
        using qi::_1;
        using qi::_2;
    	namespace phx = boost::phoenix;

    	// Semantic action for building a chemical element from a unique isotope
    	phx::function<BuildElementFromUniqueIsotope> build_element_from_unique_isotope;
        // Semantic action for validating the sum of the molar ratio of a set of isotopes
        phx::function<ValidateIsotopeContents> validate_isotopes_contents;
        // Semantic action for building a chemical element from a mixture of isotopes
    	phx::function<BuildElementFromIsotopeMixture> build_element_from_isotope_mixture;
        // Semantic action for building a chemical element from its natural isotopes
    	phx::function<BuildElementFromNaturalIsotopes> build_element_from_natural_isotopes;

        // Semantic action for building incrementally a chemical compound from a chemical element
    	phx::function<BuildCompoundFromElement> build_compound_from_element;

    	// Semantic action for validating the sum of the molar ratio of a list of compounds
        phx::function<ValidateCompoundContents> validate_compounds_contents;
        // Semantic action for building a material from a list of chemical compounds
    	phx::function<BuidMaterialFromCompounds> build_material_from_compounds;

    	// Define the isotope names and chemical symbols tokens
        IsotopeDatabaseManager* imgr=IsotopeDatabaseManager::Instance();
		const auto& isotopeDatabase=imgr->getDatabase();
		for (const auto& isotope : isotopeDatabase) {
			_isotopeToken.add(isotope.second.getName(),isotope.second.getName());
			_elementToken.add(isotope.second.getProperty<std::string>("symbol"),isotope.second.getProperty<std::string>("symbol"));
		}

		// The tokens for setting up a chemical element
        _mixtureToken = "{" > ((_isotopeToken > "(" > double_ > ")") % ","  > "}" > eps(validate_isotopes_contents(_val)));
        _isotopeMixtureToken = (_elementToken >> _mixtureToken)[_pass=build_element_from_isotope_mixture(_val,_1,_2)];
        _uniqueIsotopeToken = (_isotopeToken)[_pass=build_element_from_unique_isotope(_val,_1)];
        _naturalElementToken = (_elementToken)[_pass=build_element_from_natural_isotopes(_val,_1)];

        // The token for setting up a chemical compound
        _compoundToken = +(((_isotopeMixtureToken | _uniqueIsotopeToken | _naturalElementToken) >> (double_|attr(1.0)))[_pass=build_compound_from_element(_val,_1,_2)] );

        // The tokens for setting up a material
        _compoundMixtureToken = (((_compoundToken >> (("(" > double_ > ")") | attr(1.0))) % ";") > eps(validate_compounds_contents(_val)));
        _materialToken = (_compoundMixtureToken[_pass=build_material_from_compounds(_val,_1)]) > eoi;
    }

private:

    qi::symbols<char,std::string> _isotopeToken;
    qi::symbols<char,std::string> _elementToken;

	qi::rule<Iterator,isotopeContents()>  _mixtureToken;
	qi::rule<Iterator,isotopeContents()>  _isotopeMixtureToken;
	qi::rule<Iterator,isotopeContents()>  _uniqueIsotopeToken;
	qi::rule<Iterator,isotopeContents()>  _naturalElementToken;
	qi::rule<Iterator,isotopeContents()>  _compoundToken;
	qi::rule<Iterator,compoundList()>     _compoundMixtureToken;
	qi::rule<Iterator,isotopeContents()>  _materialToken;
};

} // Namespace Chemistry

} // Namespace SX

#endif /* NSXTOOL_FORMULAPARSER_H_ */
