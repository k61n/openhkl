#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <exception>

#include "PhysicalUnit.h"

namespace SX
{


namespace Physics
{

bool ConversionFactorOperator::operator()(double factor, physical_unit& unit) const
{
	unit.first *= factor;
	return true;
}

bool PowerOperator::operator()(physical_unit& unit, int power) const
{
	unit.first = std::pow(unit.first,power);
	std::for_each(unit.second.begin(), unit.second.end(), [power](int &element){ element*=power;});
	return true;
}

bool MultiplyOperator::operator()(physical_unit& unit1, physical_unit& unit2) const
{
	unit1.first *= unit2.first;
	std::transform(unit1.second.begin(), unit1.second.end(),unit2.second.begin(),unit1.second.begin(),std::plus<int>());
	return true;
}

bool DivideOperator::operator()(physical_unit& unit1, physical_unit& unit2) const
{
	unit1.first /= unit2.first;
	std::transform(unit1.second.begin(), unit1.second.end(),unit2.second.begin(),unit1.second.begin(),std::minus<int>());
	return true;
}

std::map<std::string,physical_unit> PhysicalUnit::_definedUnits = {{"m"  , physical_unit(1.0,    { 1, 0, 0, 0, 0, 0, 0})}, // length [meter]
															       {"s"  , physical_unit(1.0,    { 0, 1, 0, 0, 0, 0, 0})}, // time [second]
                                                                   {"K"  , physical_unit(1.0,    { 0, 0, 1, 0, 0, 0, 0})}, // temperatrure [kelvin]
                                                                   {"kg" , physical_unit(1.0,    { 0, 0, 0, 1, 0, 0, 0})}, // mass [kilogram]
                                                                   {"A"  , physical_unit(1.0,    { 0, 0, 0, 0, 1, 0, 0})}, // current [ampere]
                                                                   {"mol", physical_unit(1.0,    { 0, 0, 0, 0, 0, 1, 0})}, // substance [mole]
                                                                   {"cd" , physical_unit(1.0,    { 0, 0, 0, 0, 0, 0, 1})}, // luminosity [candela]
                                                                   {"ang", physical_unit(1.0e-10,{ 1, 0, 0, 0, 0, 0, 0})}, // angstrom
                                                                   {"J"  , physical_unit(1.0,    { 2,-2, 0, 1, 0, 0, 0})}, // joule
                                                                   {"eV" , physical_unit(1.602176565e-19,{{ 2,-2, 0, 1, 0, 0, 0}})} // electron-volt
                                                                  };

std::map<std::string,double> PhysicalUnit::_definedPrefixes = {{"y" ,1.0e-24}, //yocto
		                                                       {"z" ,1.0e-21}, // zepto
															   {"a" ,1.0e-18}, // atto
															   {"f" ,1.0e-15}, // femto
															   {"p" ,1.0e-12}, // pico
															   {"n" ,1.0e-09}, // nano
															   {"u" ,1.0e-06}, // micro
															   {"m" ,1.0e-03}, // milli
															   {"c" ,1.0e-02}, // centi
															   {"d" ,1.0e-01}, // deci
															   {"da",1.0e+01}, // deca
															   {"h" ,1.0e+02}, // hecto
															   {"k" ,1.0e+03}, // kilo
															   {"M" ,1.0e+06}, // mega
															   {"G" ,1.0e+09}, // giga
															   {"T" ,1.0e+12}, // tera
															   {"P" ,1.0e+15}, // peta
															   {"E" ,1.0e+18}, // exa
															   {"Z" ,1.0e+21}, // zeta
															   {"Y" ,1.0e+24}  // yotta
                                                              };

std::map<dimension,double> PhysicalUnit::_unitEquivalences = {{{ 2,-2, 0, 0, 0, 0, 0},1.782661907e-36}, // equivalence energy to mass
		                                                      {{ 2,-2,-1, 1, 0, 0, 0},1.1604505e+04}, // equivalence energy to temperature
		                                                      {{ 2,-3, 0, 1, 0, 0, 0},6.582119e-16}, // equivalence energy to time
		                                                      {{ 1,-2, 0, 1, 0, 0, 0},1.97327e-07}, // equivalence energy to distance
};

PhysicalUnit::PhysicalUnit(double value, const std::string& unit) : _value(value)
{
    physical_unit u;
    auto f = unit.begin();
	auto l = unit.end();
    bool ok = qi::phrase_parse(f,l,_parser,qi::space,u);
    if (ok) 
	{
		_conversionFactor = u.first;
		_dimension = u.second;
    }
    else
    	throw std::runtime_error("Invalid input unit");

}

void PhysicalUnit::addPrefix(const std::string& name, double factor)
{
	_definedPrefixes.insert(unit_prefix(name,factor));
	_parser.updatePrefixParser(name,factor);
}

void PhysicalUnit::addUnit(const std::string& name, const physical_unit& physicalUnit)
{
	_definedUnits.insert(std::make_pair(name,physicalUnit));
	_parser.updateUnitParser(name,physicalUnit);
}

double PhysicalUnit::convert(const std::string& ounit) const
{
    physical_unit u;
    auto f = ounit.begin();
	auto l = ounit.end();
    bool ok = qi::phrase_parse(f,l,_parser,qi::space,u);
    if (ok) 
	{
		bool dimEqualityTest = std::equal(_dimension.begin(),_dimension.end(),u.second.begin());
		if (dimEqualityTest)
		{
			double conversionFactor(_value*_conversionFactor/u.first);
			return conversionFactor;
		}
		else
		{
			bool equivalenceFound(false);
			for (const auto& p : _unitEquivalences)
			{
				dimension dimDifference;
				std::transform(_dimension.begin(),_dimension.end(),u.second.begin(),dimDifference.begin(), std::minus<int>());
				int prod=std::inner_product(dimDifference.begin(),dimDifference.end(),p.first.begin(),0);
				int norm2=std::inner_product(p.first.begin(),p.first.end(),p.first.begin(),0);
				double ratio=static_cast<double>(prod)/static_cast<double>(norm2);
				double intpart;
				if (std::modf(ratio,&intpart)==0.0)
				{
					double conversionFactor(_value*std::pow(p.second,ratio));
					equivalenceFound = true;
					break;
				}
			}
			if (!equivalenceFound)
		    	throw std::runtime_error("Invalid output unit");
		}
    }
    else
    	throw std::runtime_error("Invalid output unit");
}

double PhysicalUnit::getConversionFactor() const
{
	return _conversionFactor;
}

const dimension& PhysicalUnit::getDimension() const
{
	return _dimension;
}

void PhysicalUnit::setValue(double value)
{
	_value = value;
}

double PhysicalUnit::getValue() const
{
	return _value;
}

PhysicalUnit::PhysicalUnitParser::PhysicalUnitParser() : PhysicalUnitParser::base_type(_start) 
{

	namespace phx = boost::phoenix;
	phx::function<ConversionFactorOperator> const update_conversion_factor = ConversionFactorOperator();
	phx::function<PowerOperator> const powerize_unit = PowerOperator();
	phx::function<MultiplyOperator> const multiply_unit = MultiplyOperator();
	phx::function<DivideOperator> const divide_unit = DivideOperator();

	for (const auto& prefix : _definedPrefixes)
		_prefix.add(prefix.first,prefix.second);

	for (const auto& dim : _definedUnits)
		_unit.add(dim.first,dim.second);

    _prefixedUnit = ((_prefix[qi::_a=qi::_1] >> _unit[qi::_val=qi::_1]) | (qi::attr(1.0)[qi::_a=qi::_1] >> _unit[qi::_val=qi::_1]))[qi::_pass=update_conversion_factor(qi::_a,qi::_val)];

    _poweredUnit = (_prefixedUnit[qi::_val=qi::_1,qi::_a=1] >> -(qi::lit("**") >> qi::int_[qi::_a=qi::_1]))[qi::_pass=powerize_unit(qi::_val,qi::_a)];

    _compositeUnit = (_poweredUnit[qi::_val=qi::_1] >> -((qi::lit("/") >> _poweredUnit[qi::_a=qi::_1])[qi::_pass=divide_unit(qi::_val,qi::_a)] |
    		                                             (qi::lit("*") >> _poweredUnit[qi::_a=qi::_1])[qi::_pass=multiply_unit(qi::_val,qi::_a)]));

	_start = _compositeUnit[qi::_val=qi::_1];

}

void PhysicalUnit::PhysicalUnitParser::updatePrefixParser(const std::string& name, double prefix)
{
	_prefix.add(name,prefix);
}

void PhysicalUnit::PhysicalUnitParser::updateUnitParser(const std::string& name, const physical_unit& physicalUnit)
{
	_unit.add(name,physicalUnit);
}

} // end namespace Physics

} //end namespace SX

