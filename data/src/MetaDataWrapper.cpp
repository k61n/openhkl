#include "MetaDataWrapper.h"
#include <iostream>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace SX
{

namespace Data
{

PyObject* getPythonKey(MetaData* m, const std::string& key)
{
	boost::any value=m->getKey(key);

	if (value.type()==typeid(std::string))
		return Py_BuildValue("s", boost::any_cast<std::string>(value).c_str());
	else if (value.type()==typeid(double))
		return Py_BuildValue("d", boost::any_cast<double>(value));
	else if (value.type()==typeid(int))
		return Py_BuildValue("i", boost::any_cast<int>(value));
	else if (value.type()==typeid(boost::posix_time::ptime))
		return Py_BuildValue("s", boost::posix_time::to_simple_string(boost::any_cast<boost::posix_time::ptime>(value)).c_str());
	else
		throw std::runtime_error("Key type not listed");
}

PyObject* getPythonKeyAsString(MetaData* m, const std::string& key)
{
	boost::any value=m->getKey(key);

	PyObject* temp = getPythonKey(m, key);

	return PyObject_Str(temp);;

}

} // end namespace Data

} // end namespace SX
