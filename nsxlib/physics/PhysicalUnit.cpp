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

std::map<std::string,physical_unit> PhysicalUnit::_definedUnits = {{"m"  , physical_unit(1.0,    { 1, 0, 0, 0, 0, 0, 0})},
															       {"s"  , physical_unit(1.0,    { 0, 1, 0, 0, 0, 0, 0})},
                                                                   {"K"  , physical_unit(1.0,    { 0, 0, 1, 0, 0, 0, 0})},
                                                                   {"kg" , physical_unit(1.0,    { 0, 0, 0, 1, 0, 0, 0})},
                                                                   {"A"  , physical_unit(1.0,    { 0, 0, 0, 0, 1, 0, 0})},
                                                                   {"mol", physical_unit(1.0,    { 0, 0, 0, 0, 0, 1, 0})},
                                                                   {"cd" , physical_unit(1.0,    { 0, 0, 0, 0, 0, 0, 1})},
                                                                   {"ang", physical_unit(1.0e-10,{ 1, 0, 0, 0, 0, 0, 0})},
                                                                   {"J"  , physical_unit(1.0,    { 2,-2, 0, 1, 0, 0, 0})},
                                                                   {"eV" , physical_unit(1.602176565e-19,{{ 2,-2, 0, 1, 0, 0, 0}})},
};

std::map<std::string,double> PhysicalUnit::_definedPrefixes = {{"y" ,1.0e-24},
                                              {"z" ,1.0e-21},
                                              {"a" ,1.0e-18},
                                              {"f" ,1.0e-15},
                                              {"p" ,1.0e-12},
                                              {"n" ,1.0e-09},
                                              {"u" ,1.0e-06},
                                              {"m" ,1.0e-03},
                                              {"c" ,1.0e-02},
                                              {"d" ,1.0e-01},
                                              {"da",1.0e+01},
                                              {"h" ,1.0e+02},
                                              {"k" ,1.0e+03},
                                              {"M" ,1.0e+06},
                                              {"G" ,1.0e+09},
                                              {"T" ,1.0e+12},
                                              {"P" ,1.0e+15},
                                              {"E" ,1.0e+18},
                                              {"Z" ,1.0e+21},
                                              {"Y" ,1.0e+24}};

std::map<dimension,double> PhysicalUnit::_unitEquivalences = {{{2,-2,0,0,0,0,0},1.782661907e-36}};

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
			for (const auto& p : _unitEquivalences)
			{
				dimension dimDifference;
				std::transform(_dimension.begin(),_dimension.end(),u.second.begin(),dimDifference.begin(), std::minus<int>());
				int prod=std::inner_product(dimDifference.begin(),dimDifference.end(),p.first.begin(),0);
				auto sumsquare = [](double sum2,int v){sum2 += static_cast<double>(v*v);return sum2;};
				int norm2=std::inner_product(p.first.begin(),p.first.end(),p.first.begin(),0);
				double ratio=static_cast<double>(prod)/static_cast<double>(norm2);
				double intpart;
				if (std::modf(ratio,&intpart)==0.0)
				{
					double conversionFactor(_value*std::pow(p.second,ratio));
					break;
				}
			}
		}
    }
}

double PhysicalUnit::getConversionFactor() const
{
	return _conversionFactor;
}

const dimension& PhysicalUnit::getDimension() const
{
	return _dimension;
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

	for (const auto& prefix : _definedPrefixes)
		_prefix.add(prefix.first,prefix.second);

	for (const auto& dim : _definedUnits)
		_unit.add(dim.first,dim.second);

    _prefixedUnit = ((_prefix[qi::_a=qi::_1] >> _unit[qi::_val=qi::_1]) | (qi::attr(1.0)[qi::_a=qi::_1] >> _unit[qi::_val=qi::_1]))[qi::_pass=update_conversion_factor(qi::_a,qi::_val)];

    _poweredUnit = (_prefixedUnit[qi::_val=qi::_1,qi::_a=1] >> -(qi::lit("**") >> qi::int_[qi::_a=qi::_1]))[qi::_pass=powerize_unit(qi::_val,qi::_a)];

    _compositeUnit = _poweredUnit[qi::_val=qi::_1] >> -(qi::lit("*") >> _poweredUnit[qi::_a=qi::_1])[qi::_pass=multiply_unit(qi::_val,qi::_a)];

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

