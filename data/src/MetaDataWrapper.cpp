#include "MetaDataWrapper.h"
#include <boost/any.hpp>

namespace SX
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
	else
		throw std::runtime_error("Key type not listed");
}

PyObject* getPythonKeyAsString(MetaData* m, const std::string& key)
{
	boost::any value=m->getKey(key);
	return Py_BuildValue("s", boost::any_cast<std::string>(value).c_str());
}

}
