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
#ifndef NSXLIB_AFFINETRANSFORMPARSER_H
#define NSXLIB_AFFINETRANSFORMPARSER_H

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>
#include <Eigen/Dense>

namespace nsx {

struct MatrixFiller {
template <typename Matrix, typename Row, typename Col, typename Value>
bool operator()(Matrix &m, Row r, Col c, Value v) const
{
    if (r<0 || r>3 || c<0 || c>3)
        return false;

    m(r,c)+=v;
    return true;
}
};

struct MatrixInit {
template <typename Matrix>
bool operator()(Matrix &m) const
{
    m.setIdentity();
    m(0,0)=m(1,1)=m(2,2)=0;
    return true;
}
};


/*! Class to parse 3D affine transformations written as strings of the form
* 2x+1/3,y-z+1,x+1/4
* using the Jones short notation
* (Jones faithful representation) into numerical rotational and translational parts
* of the symmetry operator.
* This parser only validates the grammar but not the Symmetry
* operation itself. Valid terms are signed linear terms in x, y, or z and constants,either integer
* or fractional numbers written as n/d.
* For example, the symbol x,y,z will be parsed as the identity matrix and no translational
* part. The symbol -x,-y,z+1/2 will be parsed into the matrix
* -1,0,0,0
* 0,-1,0,0
* 0,0,1,0.5,
* 0,0,0,1
*/

template<typename It>
struct AffineTransformParser : boost::spirit::qi::grammar<It,Eigen::Transform<double,3,Eigen::Affine>()> {
    AffineTransformParser(): AffineTransformParser::base_type(matrix)
    {
        using boost::spirit::qi::_1;
        using boost::spirit::qi::_a;
        using boost::spirit::qi::_b;
        using boost::spirit::qi::_pass;
        using boost::spirit::qi::_val;
        using boost::spirit::qi::double_;
        using boost::spirit::qi::eps;
        using boost::spirit::qi::lit;
        using boost::phoenix::ref;

        boost::phoenix::function<MatrixFiller> const matrix_insert = MatrixFiller();
        boost::phoenix::function<MatrixInit> const matrix_init = MatrixInit();

        static double row=0;
        static double col=0;

        // Multiple terms, separated by commas

        matrix = eps[_pass=matrix_init(_val),ref(row)=0,ref(col)=0] >>
              *term[_pass=matrix_insert(_val,ref(row),ref(col),_1)] >> lit(',')[ref(row)=1] >>
              *term[_pass=matrix_insert(_val,ref(row),ref(col),_1)] >> lit(',')[ref(row)=2] >>
              *term[_pass=matrix_insert(_val,ref(row),ref(col),_1)];

        // either prefactor followed by x, y, z or constant term
        term = eps[_a=1.0,_b=1.0,ref(col)=0] >>
                -(lit('+')|lit('-')[_b=-1.0]) >>
                 ((-prefactor[_a=_1]) >> xyz)[_val=_a*_b] |
                 (prefactor[_val=_1])[ref(col)=3];

        // Any symbol x, y, z small or capital letters
        xyz = (
                (lit('x')|lit('X'))[ref(col)=0] |
                (lit('y')|lit('Y'))[ref(col)=1] |
                (lit('z')|lit('Z'))[ref(col)=2]);

        // Either a double or a fraction
        prefactor = eps [_val=1.0] >>
                 double_[_val*=_1] >>
                 -(lit('/') >> double_[_val/=_1]) ;
    }
        boost::spirit::qi::rule<It,Eigen::Transform<double,3,Eigen::Affine>()> matrix;
        boost::spirit::qi::rule<It,double(),boost::spirit::qi::locals<double,double> > term;
        boost::spirit::qi::rule<It> xyz;
        boost::spirit::qi::rule<It,double()> prefactor;
};

} // end namespace nsx

#endif // NSXLIB_AFFINETRANSFORMPARSER_H
