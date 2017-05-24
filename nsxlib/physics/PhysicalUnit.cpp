#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <stdexcept>

#include "PhysicalUnit.h"

namespace nsx {

bool PrefixOperator::operator()(double prefix, physical_unit& unit) const
{
    auto& currentPrefix=std::get<0>(unit);
    currentPrefix *= prefix;
    return true;
}

bool PowerOperator::operator()(physical_unit& unit, int power) const
{
    auto& currentPrefix=std::get<0>(unit);
    currentPrefix = std::pow(currentPrefix,power);

    auto& currentScale=std::get<1>(unit);
    currentScale = std::pow(currentScale,power);

    auto& dimension=std::get<2>(unit);
    std::for_each(dimension.begin(),dimension.end(), [power](int &element){ element*=power;});

    return true;
}

bool MultiplyOperator::operator()(physical_unit& unit1, const physical_unit& unit2) const
{
    auto& currentPrefix=std::get<0>(unit1);
    currentPrefix *= std::get<0>(unit2);

    auto& currentScale=std::get<1>(unit1);
    currentScale *= std::get<1>(unit2);

    auto& dimension1=std::get<2>(unit1);
    auto& dimension2=std::get<2>(unit2);
    std::transform(dimension1.begin(),dimension1.end(),dimension2.begin(),dimension1.begin(),std::plus<int>());

    return true;
}

bool DivideOperator::operator()(physical_unit& unit1, const physical_unit& unit2) const
{
    auto& currentPrefix=std::get<0>(unit1);
    currentPrefix *= std::get<0>(unit2);

    auto& currentScale=std::get<1>(unit1);
    currentScale *= std::get<1>(unit2);

    auto& dimension1=std::get<2>(unit1);
    auto& dimension2=std::get<2>(unit2);
    std::transform(dimension1.begin(),dimension1.end(),dimension2.begin(),dimension1.begin(),std::minus<int>());

    return true;
}

std::map<std::string,physical_unit> PhysicalUnit::_definedUnits = {
    {"m"  , physical_unit(1.0    ,1.0,            {{ 1, 0, 0, 0, 0, 0, 0}})}, // length [meter]
    {"s"  , physical_unit(1.0    ,1.0,            {{ 0, 1, 0, 0, 0, 0, 0}})}, // time [second]
    {"K"  , physical_unit(1.0    ,1.0,            {{ 0, 0, 1, 0, 0, 0, 0}})}, // temperature [kelvin]
    {"kg" , physical_unit(1.0    ,1.0,            {{ 0, 0, 0, 1, 0, 0, 0}})}, // mass [kilogram]
    {"A"  , physical_unit(1.0    ,1.0,            {{ 0, 0, 0, 0, 1, 0, 0}})}, // current [ampere]
    {"mol", physical_unit(1.0    ,1.0,            {{ 0, 0, 0, 0, 0, 1, 0}})}, // substance [mole]
    {"cd" , physical_unit(1.0    ,1.0,            {{ 0, 0, 0, 0, 0, 0, 1}})}, // luminosity [candela]
    {"ang", physical_unit(1.0e-10,1.0,            {{ 1, 0, 0, 0, 0, 0, 0}})}, // distance [angstrom]
    {"J"  , physical_unit(1.0    ,1.0,            {{ 2,-2, 0, 1, 0, 0, 0}})}, // energy [joule]
    {"eV" , physical_unit(1.0    ,1.602176565e-19,{{ 2,-2, 0, 1, 0, 0, 0}})}, // energy [electron-volt]
    {"min", physical_unit(1.0    ,60.0           ,{{ 0, 1, 0, 0, 0, 0, 0}})}, // time [minute]
    {"h"  , physical_unit(1.0    ,3660.0         ,{{ 0, 1, 0, 0, 0, 0, 0}})}, // time [hour]
    {"g"  , physical_unit(1.0e-3 ,1.0            ,{{ 0, 0, 0, 1, 0, 0, 0}})}, // time [gram]
};

std::map<std::string,double> PhysicalUnit::_definedPrefixes = {
    {"y" ,1.0e-24}, //yocto
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

std::map<dimension,double> PhysicalUnit::_unitEquivalences = {
    {{{ 2,-2, 0, 0, 0, 0, 0}},1.1126500948414508e-17}, // energy to mass (J <--> kg)
    {{{ 2,-2,-1, 1, 0, 0, 0}},7.242971666667e+22},     // energy to temperature (J <--> K)
    {{{ 2,-3, 0, 1, 0, 0, 0}},4108.235723695035},      // energy to time (J <--> s)
    {{{ 1,-2, 0, 1, 0, 0, 0}},1.2316183141775015e12},  // energy to distance (J <--> m)
};

PhysicalUnit::PhysicalUnit(const PhysicalUnit& other)
: _value(other._value),
  _prefix(other._prefix),
  _scale(other._scale),
  _dimension(other._dimension)
{
}

PhysicalUnit::PhysicalUnit(double value, const std::string& unit) : _value(value)
{
    physical_unit u;
    auto f = unit.begin();
    auto l = unit.end();
    bool ok = boost::spirit::qi::phrase_parse(f,l,_parser,boost::spirit::qi::space,u);
    if (ok)
    {
        _prefix = std::get<0>(u);
        _scale = std::get<1>(u);
        _dimension = std::get<2>(u);
    }
    else
        throw std::runtime_error("Invalid input unit");

}

PhysicalUnit::PhysicalUnit(const std::string& unit) : _value(1.0)
{
    physical_unit u;
    auto f = unit.begin();
    auto l = unit.end();
    bool ok = boost::spirit::qi::phrase_parse(f,l,_parser,boost::spirit::qi::space,u);
    if (ok)
    {
        _prefix = std::get<0>(u);
        _scale = std::get<1>(u);
        _dimension = std::get<2>(u);
    }
    else
        throw std::runtime_error("Invalid input unit");

}

PhysicalUnit::PhysicalUnit(double value, double prefix, double scale, const dimension& dimension)
: _value(value),
  _prefix(prefix),
  _scale(scale),
  _dimension(dimension)
{
}

PhysicalUnit::PhysicalUnit(double prefix, double scale, const dimension& dimension)
: _value(1.0),
  _prefix(prefix),
  _scale(scale),
  _dimension(dimension)
{
}

PhysicalUnit& PhysicalUnit::operator=(const PhysicalUnit& other)
{
    if (this!=&other)
    {
        _value = other._value;
        _prefix = other._prefix;
        _scale = other._scale;
        _dimension = other._dimension;
    }
    return *this;
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
    bool ok = boost::spirit::qi::phrase_parse(f,l,_parser,boost::spirit::qi::space,u);
    if (ok)
        return convert(std::get<0>(u),std::get<1>(u),std::get<2>(u));
    else
        throw std::runtime_error("Invalid output unit");
}

double PhysicalUnit::convert(double oprefix, double oscale, const dimension& odimension) const
{
    bool dimEqualityTest = std::equal(_dimension.begin(),_dimension.end(),odimension.begin());
    if (dimEqualityTest)
    {
        double conversionFactor(_value*_prefix*_scale/(oprefix*oscale));
        return conversionFactor;
    }
    else
    {
        dimension dimDifference;

        std::transform(_dimension.begin(),_dimension.end(),odimension.begin(),dimDifference.begin(), std::minus<int>());

        for (const auto& p : _unitEquivalences)
        {
            auto it1=dimDifference.begin();
            bool proportional=true;
            for (auto it2=p.first.begin();it2!=p.first.end();++it1,++it2)
            {
                if ((*it1)!=0 ^ (*it2)!=0)
                {
                    proportional=false;
                    break;
                }
            }

            if (!proportional)
                continue;

            int prod=std::inner_product(dimDifference.begin(),dimDifference.end(),p.first.begin(),0);
            int norm2=std::inner_product(p.first.begin(),p.first.end(),p.first.begin(),0);
            double ratio=static_cast<double>(prod)/static_cast<double>(norm2);
            double intpart;
            if (std::abs(std::modf(ratio,&intpart))<1.0e-9)
            {
                double conversionFactor(_value*std::pow(p.second,ratio)*_prefix*_scale/(oprefix*oscale));
                return conversionFactor;
            }
        }

        throw std::runtime_error("Invalid output unit");
    }
}

double PhysicalUnit::getPrefix() const
{
    return _prefix;
}

double PhysicalUnit::getScale() const
{
    return _scale;
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

double PhysicalUnit::convertToSI() const
{
    return _prefix*_scale*_value;
}

PhysicalUnit::PhysicalUnitParser::PhysicalUnitParser() : PhysicalUnitParser::base_type(_start)
{

    using boost::spirit::qi::_1;
    using boost::spirit::qi::_a;
    using boost::spirit::qi::_pass;
    using boost::spirit::qi::_val;
    using boost::spirit::qi::attr;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::lit;

    boost::phoenix::function<PrefixOperator> const update_prefix = PrefixOperator();
    boost::phoenix::function<PowerOperator> const powerize_unit = PowerOperator();
    boost::phoenix::function<MultiplyOperator> const multiply_unit = MultiplyOperator();
    boost::phoenix::function<DivideOperator> const divide_unit = DivideOperator();

    for (const auto& prefix : _definedPrefixes)
        _prefix.add(prefix.first,prefix.second);

    for (const auto& dim : _definedUnits)
        _unit.add(dim.first,dim.second);

    _prefixedUnit = ((_prefix[_a=_1] >> _unit[_val=_1]) | (attr(1.0)[_a=_1] >> _unit[_val=_1]))[_pass=update_prefix(_a,_val)];

    _poweredUnit = (_prefixedUnit[_val=_1,_a=1] >> -(lit("**") >> int_[_a=_1]))[_pass=powerize_unit(_val,_a)];

    _compositeUnit = (_poweredUnit[_val=_1] >> -((lit("/") >> _poweredUnit[_a=_1])[_pass=divide_unit(_val,_a)] |
                                                         (lit("*") >> _poweredUnit[_a=_1])[_pass=multiply_unit(_val,_a)]));

    _start = _compositeUnit[_val=_1];

}

void PhysicalUnit::PhysicalUnitParser::updatePrefixParser(const std::string& name, double prefix)
{
    _prefix.add(name,prefix);
}

void PhysicalUnit::PhysicalUnitParser::updateUnitParser(const std::string& name, const physical_unit& physicalUnit)
{
    _unit.add(name,physicalUnit);
}

} // end namespace nsx

