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

#include <tuple>
#include <set>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>

namespace nsx {
namespace Utils {

namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;

using constraint_tuple = std::tuple<unsigned int,unsigned int,double>;
using constraints_set = std::set<constraint_tuple>;

struct SingleConstraint {
    template <typename constraint_tuple>
    bool operator()(constraint_tuple &constraint, int lhs, int rhs, double coeff) const
    {
        std::get<0>(constraint) = lhs;
        std::get<1>(constraint) = rhs;
        std::get<2>(constraint) = coeff;
        return true;
    }
};

struct MultiConstraint {
    template <typename constraint_set>
    bool operator()(constraint_set &constraints, const constraint_tuple& singleConstraint) const
    {

        constraints.insert(singleConstraint);

        return true;
    }
};

template<typename It>
struct LatticeConstraintParser : qi::grammar<It,constraints_set()> {

    LatticeConstraintParser(): LatticeConstraintParser::base_type(constraints)
    {
        using namespace qi;
        using namespace phx;
        using qi::_1;

        phx::function<SingleConstraint> const set_single_constraint = SingleConstraint();
        phx::function<MultiConstraint> const add_single_constraint = MultiConstraint();

        constraints= (single_constraint[_pass=add_single_constraint(_val,_1)] >> *(lit(',') >> single_constraint[_pass=add_single_constraint(_val,_1)]));
        single_constraint = (eps[_a=1.0,_b=1.0] >> -(prefactor[_a*=_1]) >> param[_c=_1] >> lit('=') >> -(prefactor[_b*=_1]) >> param[_d=_1])[_pass=set_single_constraint(_val,_c,_d,_b/_a)];
        param = (string("alpha")[_val=3]|string("beta")[_val=4]|string("gamma")[_val=5]|lit('a')[_val=0]|lit('b')[_val=1]|lit('c')[_val=2]);
        prefactor = eps[_val=1.0] >> double_[_val*=_1];
    }

    qi::rule<It,constraints_set()> constraints;
    qi::rule<It,constraint_tuple(),qi::locals<double,double,unsigned int,unsigned int>> single_constraint;
    qi::rule<It,unsigned int()> param;
    qi::rule<It,double()> prefactor;
};

} // Namespace Utils
} // Namespace SX

#endif /* NSXTOOL_LATTICECONSTRAINTPARSER_H_ */
