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

#pragma once

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>

#include "UtilsTypes.h"

namespace nsx {

struct SingleConstraint {
    template <typename ConstraintTuple>
    bool operator()(ConstraintTuple &constraint, int lhs, int rhs, double coeff) const
    {
        std::get<0>(constraint) = lhs;
        std::get<1>(constraint) = rhs;
        std::get<2>(constraint) = coeff;
        return true;
    }
};

struct MultiConstraint {
    template <typename ConstraintSet>
    bool operator()(ConstraintSet &constraints, const ConstraintTuple& singleConstraint) const
    {

        constraints.insert(singleConstraint);

        return true;
    }
};

template<typename It>
struct LatticeConstraintParser : boost::spirit::qi::grammar<It,ConstraintSet()> {

    LatticeConstraintParser(): LatticeConstraintParser::base_type(constraints)
    {
        using boost::spirit::qi::_1;
        using boost::spirit::qi::_a;
        using boost::spirit::qi::_b;
        using boost::spirit::qi::_c;
        using boost::spirit::qi::_d;
        using boost::spirit::qi::_pass;
        using boost::spirit::qi::_val;
        using boost::spirit::qi::double_;
        using boost::spirit::qi::eps;
        using boost::spirit::qi::lit;
        using boost::spirit::qi::string;

        boost::phoenix::function<SingleConstraint> const set_single_constraint = SingleConstraint();
        boost::phoenix::function<MultiConstraint> const add_single_constraint = MultiConstraint();

        constraints = (singleConstraint[_pass=add_single_constraint(_val,_1)] >> *(lit(',') >> singleConstraint[_pass=add_single_constraint(_val,_1)]));
        singleConstraint = (eps[_a=1.0,_b=1.0] >> -(prefactor[_a*=_1]) >> param[_c=_1] >> lit('=') >> -(prefactor[_b*=_1]) >> param[_d=_1])[_pass=set_single_constraint(_val,_c,_d,_b/_a)];
        param = (string("alpha")[_val=3]|string("beta")[_val=4]|string("gamma")[_val=5]|lit('a')[_val=0]|lit('b')[_val=1]|lit('c')[_val=2]);
        prefactor = eps[_val=1.0] >> double_[_val*=_1];
    }

    boost::spirit::qi::rule<It,ConstraintSet()> constraints;
    boost::spirit::qi::rule<It,ConstraintTuple(),boost::spirit::qi::locals<double,double,unsigned int,unsigned int>> singleConstraint;
    boost::spirit::qi::rule<It,unsigned int()> param;
    boost::spirit::qi::rule<It,double()> prefactor;
};

} // end namespace nsx
