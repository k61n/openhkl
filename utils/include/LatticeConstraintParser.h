/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
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
#ifndef NSXTOOL_AFFINETRANSFORMPARSER_H_
#define NSXTOOL_AFFINETRANSFORMPARSER_H_
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>

namespace qi    = boost::spirit::qi;
namespace phx  = boost::phoenix;

namespace SX
{
namespace Utils
{


/*! Class to parse 3D affine transformations written as strings of the form
*/

template<typename It>
struct LatticeConstraintParser : qi::grammar<It>
{
	LatticeConstraintParser(): LatticeConstraintParser::base_type(multiterm)
    {
        using namespace qi;
        using namespace phx;
        static int par=0;

        multiterm= (term >> +(lit(',') >> term));
        term = (value | constraint);
        constraint = (-double_ >> param >> lit('=') >> (param | (double_ >>param)));
        value = (param >> lit('=') >> double_>>eoi);
        param =(lit('a')[ref(par)=0]|lit('b')[ref(par)=1]|lit('c')[ref(par)=2]|string("alpha")[ref(par)=3]|string("beta")[ref(par)=4]|string("gamma")[ref(par)=5]);
    }
		qi::rule<It> multiterm;
		qi::rule<It> term;
        qi::rule<It,double(),double()> constraint;
        qi::rule<It,double()> value;
        qi::rule<It> param;
};

} // Namespace Utils

} // Namespace SX

#endif /* NSXTOOL_JONESSYMBOLPARSER_H_ */

