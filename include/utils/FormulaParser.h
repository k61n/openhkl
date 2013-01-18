/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr

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

namespace SX
{
	using namespace boost::spirit;
	//! Element symbol, isotope (0 if natural), and content
	typedef boost::fusion::vector3<std::string,unsigned int,double> element;
	//! Formula as a vector of elements
	typedef std::vector<element> formula;
	//! @Class: ChemFormulaParser. Use to parse a chemical formula including isotopes.
	//! The chemical formula is given following the usual conventions with isotopes in brackets.
	//! For example Gd(156)Mn2O5. This does not check the nature of the chemical element
	//! or isotope validity. This is left to the periodic table.
	//! The parser is used as usual in spirit given two iterators (begin and end)
	//! : parse(begin,end,parser)
	template <typename Iterator>
	struct FormParser : qi::grammar<Iterator,formula() >
	{
		FormParser(): FormParser::base_type(start)
			{
			using boost::spirit::uint_;
			using boost::spirit::ascii::char_;
			using boost::spirit::double_;
			//! Formula is made of multiple elements.
			start=*(startelement);
			//! Symbol followed optionally by isotope, and by content.
			startelement = symbol >> (isot | qi::attr(0))>> (double_ | qi::attr(1.0));
			//! if isotope is given, should be inside bracket
			isot= ( boost::spirit::omit[char_("(")]>> uint_ >> boost::spirit::omit[char_(")")]);
			//! Capital letter followed optionally by small character
			symbol = (char_("A-Z") >> -(char_("a-z")));
			}
	private:
		qi::rule<Iterator,formula()> start;
		qi::rule<Iterator,element()> startelement;
		qi::rule<Iterator,unsigned int()> isot;
		qi::rule<Iterator,std::string()> symbol;
	};

} // Namespace SX


#endif /* NSXTOOL_FORMULAPARSER_H_ */
