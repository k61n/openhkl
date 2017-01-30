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

#ifndef NSXTOOL_PHYSICALUNIT_H_
#define NSXTOOL_PHYSICALUNIT_H_

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <array>
#include <iostream>
#include <ratio>
#include <map>
#include <string>
#include <tuple>

namespace qi = boost::spirit::qi;

namespace SX
{

namespace Physics
{

// Define a dimension as a 7 elements array, each element corresponding to one of the SI unit
// The order is the following [m,s,K,kg,A,mol,cd]
using dimension = std::array<int,7>;
// Define one of the unit prefix
using unit_prefix = std::pair<std::string,double>;
// Define a physical unit as a tuple of the form <prefix,scale,dimension> where
// - prefix is a unit prefix (e.g. k=kilo=1.0e-3)
// - scale is a factor that scale the unit from its SI corresponding unit (e.g. angstrom=1.e-10*m)
// - dimension is the array of the dimensions of its corresponding SI unit
using physical_unit = std::tuple<double,double,dimension>;

//! @class: PrefixOperator.
//! Apply a prefix to a given physical unit.
struct PrefixOperator
{
    bool operator()(double factor, physical_unit& unit) const;
};

//! @class: PowerOperator.
//! Apply the power operator to a given physical unit.
struct PowerOperator
{
    bool operator()(physical_unit& unit, int power) const;
};

//! @class: MultiplyOperator.
//! Multiply a physical unit by another one.
struct MultiplyOperator
{
    bool operator()(physical_unit& unit1, const physical_unit& unit2) const;
};

//! @class: DivideOperator.
//! Divide a physical unit by another one.
struct DivideOperator
{
    bool operator()(physical_unit& unit1, const physical_unit& unit2) const;
};

//! @class: PhysicalUnit.
//! Defines a physical unit.
//! A physical unit is defined by setting a value, a prefix, a scale factor and a dimension.
//! When converting from one physical unit to another a dimension analysis is performed.
//! The analysis is based on https://www.cs.utexas.edu/users/novak/units95.pdf
//! For a physical unit to be convertible to another physical unit, the two units must either:
//! - have the same dimension
//! - have an equivalence registered
class PhysicalUnit
{
private:
    //! Stores the prefixes defined so far
    static std::map<std::string,double> _definedPrefixes;
    //! Stores the uits defined so far
    static std::map<std::string,physical_unit> _definedUnits;
    //! Stores the equivalences defined so far
    static std::map<dimension,double> _unitEquivalences;

public:

    //! Default constructor (deleted)
    PhysicalUnit()=delete;

    //! Copy constructor
    PhysicalUnit(const PhysicalUnit& other);

    //! Construct a physical unit from a prefix, a scale and a dimension
    //! The value is set to 1.0 by default
    PhysicalUnit(double prefix, double scale, const dimension& dimension);

    //! Construct a physical unit from a value, a prefix, a scale and a dimension
    PhysicalUnit(double value, double prefix, double scale, const dimension& dimension);

    //! Construct a physical unit from a string encoding the unit
    //! The value is set to 1.0 by default
    //! The string encoding the unit will be parsed by the internal boost::spirit parser
    PhysicalUnit(const std::string& unit);

    //! Construct a physical unit from a value and a string encoding the unit
    //! The string encoding the unit will be parsed by the internal boost::spirit parser
    PhysicalUnit(double value, const std::string& unit);

    //! Assignment operator
    PhysicalUnit& operator=(const PhysicalUnit& other);

    //! Set the value of the physical unit
    void setValue(double value);
    //! Return the value of the physical unit
    double getValue() const;

    //! Return the prefix of the physical unit
    double getPrefix() const;

    //! Return the scale of the physical unit
    double getScale() const;

    //! Return the dimension of the physical unit
    const dimension& getDimension() const;

    //! Return the value of the physical unit converted into SI unit
    double convertToSI() const;

    //! Add a new prefix to the prefixes defined statically so far
    void addPrefix(const std::string& name, double factor);

    //! Add a new unit to the prefixes defined statically so far
    void addUnit(const std::string& name, const physical_unit& physicalUnit);

    //! Return the value of the conversion from the physical unit to another one
    double convert(const std::string& ounit) const;

    //! Return the value of the conversion from the physical unit to another one
    double convert(double oprefix, double oscale, const dimension& odimension) const;

private:
    //! Inner class that defines the boost::spirit parser for a physical unit
    struct PhysicalUnitParser : qi::grammar<std::string::const_iterator, physical_unit()>
    {
        //! Default constructor
        PhysicalUnitParser();

        //! Copy constructor (deleted)
        PhysicalUnitParser(const PhysicalUnitParser& other)=delete;

        //! Assignment operator (deleted)
        PhysicalUnitParser& operator=(const PhysicalUnitParser& other)=delete;

    public:
        //! Update the symbols defined so far for the prefixes
        void updatePrefixParser(const std::string& name, double prefix);

        //! Update the symbols defined so far for the physical units
        void updateUnitParser(const std::string& mame, const physical_unit& physicalUnit);

    private:
        //! Defines the rule for matching a prefix
        qi::symbols<char,double> _prefix;
        //! Defines the rule for matching a unit
        qi::symbols<char,physical_unit> _unit;
        //! Defines the rule for matching a prefixed unit
        qi::rule<std::string::const_iterator,physical_unit(),qi::locals<double>> _prefixedUnit;
        //! Defines the rule for matching a prefixed unit with an integer exponent
        qi::rule<std::string::const_iterator,physical_unit(),qi::locals<int>> _poweredUnit;
        //! Defines the rule for matching an arithmetic expression of several physical units
        qi::rule<std::string::const_iterator,physical_unit(),qi::locals<physical_unit>> _compositeUnit;
        //! The starting rule
        qi::rule<std::string::const_iterator,physical_unit()> _start;
    };

private:

    PhysicalUnitParser _parser;
    double _value;
    double _prefix;
    double _scale;
    dimension _dimension;

};

} // end namespace Physics

} // end namespace SX

#endif /* NSXTOOL_PHYSICALUNIT_H_ */

