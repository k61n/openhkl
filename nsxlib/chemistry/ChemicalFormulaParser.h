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

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <string>
#include <list>
#include <map>

#include <iostream>

#include "Element.h"
#include "Isotope.h"

#include "ChemicalDatabaseManager.h"

namespace SX {

namespace Chemistry {

namespace qi = boost::spirit::qi;

using isotopeMixture = std::map<std::string,double>;
using compoundMixture = std::vector<std::pair<isotopeMixture,double>>;

struct NaturalElementBuilder
{
	bool operator()(isotopeMixture &output, const std::string& elementSymbol) const
	{
		ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
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

struct IsotopeMixtureBuilder {

	bool operator()(isotopeMixture& output, const std::string& elementSymbol, const isotopeMixture& mixture) const {

		ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();

		double sumRatio(0.0);
		for (const auto& p : mixture) {
			if (p.second < 0.0 || p.second > 1.0) {
				return false;
			}
			const auto& isotope = imgr->getChemicalObject(p.first);
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

struct UpdateElement
{
	bool operator()(isotopeMixture &output, const isotopeMixture& element, double stoichiometry) const
	{
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

struct UpdateMaterial
{
	bool operator()(isotopeMixture &output, const compoundMixture& compounds) const
	{

		for (const auto& compound : compounds) {
			for (const auto& isotope : compound.first) {
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

struct PureIsotopeBuilder
{
	bool operator()(isotopeMixture &output, const std::string& isotopeName) const
	{
		output.insert(std::make_pair(isotopeName,1.0));

		return true;
	}
};

struct ValidateIsotopeMixture {

	bool operator()(const isotopeMixture& mixture) const {
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

struct ValidateCompoundMixture {

	bool operator()(const compoundMixture& mixture) const {
		double sumRatio(0.0);
		for (const auto& p : mixture) {
			for (const auto& pp : p.first) {
				std::cout<<pp.first<<" rrrr "<<pp.second<<std::endl;
			}
			if (p.second < 0.0 || p.second > 1.0) {
				return false;
			}
			sumRatio += p.second;
		}
		std::cout<<"Validate = "<<sumRatio<<std::endl;
        return (std::abs(1.0 - sumRatio) < 1.0e-6);
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
struct ChemicalFormulaParser : qi::grammar<Iterator,isotopeMixture()>
{
    ChemicalFormulaParser(): ChemicalFormulaParser::base_type(_materialToken)
    {
        using namespace qi;
    	namespace phx = boost::phoenix;

    	// Semantic action for handling the case of pure isotope
    	phx::function<PureIsotopeBuilder> build_pure_isotope;
    	// Semantic action for handling the case of pure isotope mixture
    	phx::function<IsotopeMixtureBuilder> build_isotopes_mixture;
    	// Semantic action for handling the case of natural element
    	phx::function<NaturalElementBuilder> build_natural_element;
    	phx::function<UpdateElement> update_element;
    	phx::function<UpdateMaterial> update_material;
    	phx::function<ValidateIsotopeMixture> validate_isotopes_mixture;
    	phx::function<ValidateCompoundMixture> validate_compounds_mixture;

		ChemicalDatabaseManager<Isotope>* imgr=ChemicalDatabaseManager<Isotope>::Instance();
		const auto& isotopeDatabase=imgr->getDatabase();
		for (const auto& isotope : isotopeDatabase) {
			_isotopeToken.add(isotope.second.getName(),isotope.second.getName());
			_elementToken.add(isotope.second.getProperty<std::string>("symbol"),isotope.second.getProperty<std::string>("symbol"));
		}

        _mixtureToken = "{" > ((_isotopeToken > "(" > double_ > ")") % ","  > "}" > eps(validate_isotopes_mixture(_val)));

        _isotopeMixtureToken = (_elementToken >> _mixtureToken)[_pass=build_isotopes_mixture(_val,qi::_1,qi::_2)];

        _pureIsotopeToken = (_isotopeToken)[_pass=build_pure_isotope(_val,qi::_1)];

        _naturalElementToken = (_elementToken)[_pass=build_natural_element(_val,qi::_1)];

        _compoundToken = +(((_isotopeMixtureToken | _pureIsotopeToken | _naturalElementToken) >>
        		     (double_|qi::attr(1.0)))[_pass=update_element(_val,qi::_1,qi::_2)] );

        _compoundMixtureToken = (((_compoundToken >> (("(" > double_ > ")") | qi::attr(1.0))) % ";") > eps(validate_compounds_mixture(_val)));

        _materialToken = (_compoundMixtureToken[_pass=update_material(_val,qi::_1)]) > qi::eoi;

    }

private:

    //! Defines the rule for matching a prefix
    qi::symbols<char,std::string> _isotopeToken;
    qi::symbols<char,std::string> _elementToken;

	qi::rule<Iterator,isotopeMixture()>  _mixtureToken;
	qi::rule<Iterator,isotopeMixture()>  _isotopeMixtureToken;
	qi::rule<Iterator,isotopeMixture()>  _pureIsotopeToken;
	qi::rule<Iterator,isotopeMixture()>  _naturalElementToken;
	qi::rule<Iterator,isotopeMixture()>  _compoundToken;
	qi::rule<Iterator,compoundMixture()> _compoundMixtureToken;
	qi::rule<Iterator,isotopeMixture()>  _materialToken;
};

} // Namespace Chemistry

} // Namespace SX

#endif /* NSXTOOL_FORMULAPARSER_H_ */
