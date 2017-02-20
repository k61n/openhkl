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

#include <boost/fusion/include/vector10.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <string>
#include <vector>

namespace SX {
namespace Chemistry {

namespace qi = boost::spirit::qi;
using element = boost::fusion::vector3<std::string,std::string,double>;
using formula = std::vector<element>;

//! @Class: ChemicalFormulaParser.
//! Parses a chemical formula.
//! The chemical formula is defined as a string that represents a sequence of elements (e.g. CH4, H2O, C[12]3H[2]8).
//! Each element is in turn defined by a string that represents its chemical symbol as defined in the periodic table respectively followed
//! optionally by the element isotope given in square brackets and by the number of such element in the chemical formula (1 if not provided).
//! The parser is used as usual in spirit given two iterators (begin and end) : parse(begin,end,parser)
//! Example: C3H[2]8 will parse a molecule with three natural  carbons and eight deuterium atoms i.e. a deuterated propane.
template <typename Iterator>
struct ChemicalFormulaParser : qi::grammar<Iterator,formula() >
{
    ChemicalFormulaParser(): ChemicalFormulaParser::base_type(start)
    {
        using boost::spirit::double_;
        using boost::spirit::ascii::char_;
        // A chemical formula is made of multiple elements parsing block.
        start=*(startelement);
        // Each element parsing block is made of the element name followed optionally by the number of such isotopes in the chemical
        // formula
        startelement = symbol >> isotope >> (double_ | qi::attr(1));
        //! An element name is made of a chemical symbol followed optionally by the a specific isotope definintion given in square bracket (e.g. H[2] for deuterium)
        symbol = char_("A-Z") >> -char_("a-z");
        isotope = (char_("[") >> +char_("0-9") >> char_("]")) | boost::spirit::qi::attr("");
    }
    private:
    qi::rule<Iterator,formula()> start;
    qi::rule<Iterator,element()> startelement;
    qi::rule<Iterator,std::string()> symbol;
    qi::rule<Iterator,std::string()> isotope;
};
} // Namespace Chemistry
} // Namespace SX

#endif /* NSXTOOL_FORMULAPARSER_H_ */
