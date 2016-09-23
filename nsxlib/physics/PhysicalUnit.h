#ifndef NSXTOOL_UNITPARSER_H_
#define NSXTOOL_UNITPARSER_H_

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <array>
#include <iostream>
#include <ratio>
#include <string>

namespace qi = boost::spirit::qi;

namespace Units 
{

using dimension = std::array<int,7>;
using unit_prefix = std::pair<std::string,double>;
using physical_unit = std::pair<double,dimension>;

struct ConversionFactorOperator
{
	bool operator()(double factor, physical_unit& unit) const;
};

struct PowerOperator
{
	bool operator()(physical_unit& unit, int power) const;
};

class PhysicalUnit
{
private:
	static std::map<std::string,double> _definedPrefixes;
	static std::map<std::string,physical_unit> _definedUnits;
	static std::map<dimension,double> _unitEquivalences;

public:
	PhysicalUnit(double value, const std::string& unit);

	double getConversionFactor() const;

	const dimension& getDimension() const; 

	double getValue() const;

	void addPrefix(const std::string& name, double factor);

	void addUnit(const std::string& name, const physical_unit& physicalUnit);

	double convert(const std::string& ounit) const;

private:
	struct PhysicalUnitParser : qi::grammar<std::string::const_iterator, physical_unit()> 
	{
		PhysicalUnitParser();

	public:
		void updatePrefixParser(const std::string& name, double prefix);
		void updateUnitParser(const std::string& mame, const physical_unit& physicalUnit);

	private:
		qi::symbols<char,double> _prefix;
		qi::symbols<char,physical_unit> _unit;
		qi::rule<std::string::const_iterator,physical_unit(),qi::locals<double>> _prefixedUnit;
		qi::rule<std::string::const_iterator,physical_unit(),qi::locals<int>> _poweredUnit;
		qi::rule<std::string::const_iterator,physical_unit()> _start;
	};

private:
	PhysicalUnitParser _parser;

	double _value;

	double _conversionFactor;

	dimension _dimension;


};


}

#endif /* NSXTOOL_UNITPARSER_H_ */

