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
#ifndef NSXTOOL_LATTICECONSTRAINTPARSER_H_
#define NSXTOOL_LATTICECONSTRAINTPARSER_H_
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <map>
#include <utility>

#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>

namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;

namespace SX
{

namespace Utils
{

typedef boost::tuple<int,int,double> constraint_tuple;

struct ConstraintSetter
{
	template <typename constraint_tuple>
	bool operator()(constraint_tuple &constraint, int lhs, int rhs, double coeff) const
	{

		boost::get<0>(constraint) = lhs;
		boost::get<1>(constraint) = rhs;
		boost::get<2>(constraint) = coeff;

		return true;
	}
};

template<typename It>
struct LatticeConstraintParser : qi::grammar<It,constraint_tuple()>
{
	LatticeConstraintParser(): LatticeConstraintParser::base_type(constraint)
    {
        using namespace qi;
        using namespace phx;

        phx::function<ConstraintSetter> const add_constraint = ConstraintSetter();

//        constraints= (constraint >> *(lit(',') >> constraint));
        constraint = (eps[_a=1.0,_b=1.0] >> -(prefactor[_a*=_1]) >> param[_c=_1] >> lit('=') >> -(prefactor[_b*=_1]) >> param[_d=_1])[_pass=add_constraint(_val,_c,_d,_b/_a)];
        param =(lit('a')[_val=0]|lit('b')[_val=1]|lit('c')[_val=2]|string("alpha")[_val=3]|string("beta")[_val=4]|string("gamma")[_val=5]);
        prefactor = eps[_val=1.0] >> double_[_val*=_1];
    }
	qi::rule<It,constraint_tuple(),qi::locals<double,double,int,int> > constraint;
	qi::rule<It,int()> param;
	qi::rule<It,double()> prefactor;
};

} // Namespace Utils

} // Namespace SX

#endif /* NSXTOOL_LATTICECONSTRAINTPARSER_H_ */

