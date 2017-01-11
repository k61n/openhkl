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

#ifndef NSXTOOL_COMPLEXPARSER_H_
#define NSXTOOL_COMPLEXPARSER_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <complex>
#include <string>

namespace SX
{

namespace Utils
{

//! Parse an expression for complex number of the type a+bi
template <typename Iterator>
bool strToComplex(Iterator first, Iterator last, std::complex<double>& c)
{
    using boost::spirit::qi::double_;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::ascii::space;
    using boost::phoenix::ref;

    double rN = 0.0;
    double iN = 0.0;
    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
                double_[ref(rN) = _1]
                    >> -(double_[ref(iN) = _1]) >> 'i'
            |   double_[ref(rN) = _1]
        ),
        //  End grammar

        space);

    if (!r || first != last)
        return false;
    c = std::complex<double>(rN, iN);
    return r;
}

template <typename T>
std::string complexToString(const std::complex<T>& number, double tolerance=1.0e-6)
{
    if (std::norm(number) <= tolerance)
        return "0";

    std::string v("");

    if (std::abs(number.real()) > tolerance)
        v += std::to_string(number.real());

    if (std::abs(number.imag()) > tolerance) {
        if (number.imag() > 0)
            v += " + ";

        v += std::to_string(number.imag()) + "i";
    }

    return v;
}



} // end namespace Utils

} // Namespace SX


#endif /* NSXTOOL_COMPLEXPARSER_H_ */
