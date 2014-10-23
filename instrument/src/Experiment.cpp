#include "Experiment.h"
#include "IData.h"

namespace SX
{

namespace Instrument
{

Experiment::Experiment(const Experiment& other) : _name(other._name), _diffractometer(other._diffractometer)
{
}

Experiment::Experiment(const std::string& name, std::shared_ptr<Diffractometer> diffractometer) : _name(name), _diffractometer(diffractometer)
{
}

Experiment::~Experiment() {
}

Experiment& Experiment::operator=(const Experiment& other)
{
	if (this != &other)
	{
		_name = other._name;
		_diffractometer = other._diffractometer;
	}
	return *this;
}

std::shared_ptr<Diffractometer> Experiment::getDiffractometer() const
{
	return _diffractometer;
}

std::vector<std::string> Experiment::getDataNames() const
{
	std::vector<std::string> v;
	v.reserve(_data.size());
	std::for_each(_data.begin(),_data.end(),[&v]
	                                         (const std::map<std::string,IData*>::value_type& p)
	                                         {v.push_back(p.first);});

	return v;
}

std::string Experiment::getName() const
{
	return _name;
}

void Experiment::addData(IData* data)
{

	// Add the data only if it does not exist in the current data map
	std::string basename=data->getBasename();
	auto it=_data.find(basename);
	if (it != _data.end())
		return;

	_data.insert(std::pair<std::string,IData*>(basename,data));

}

} // end namespace Instrument

} // end namespace SX
