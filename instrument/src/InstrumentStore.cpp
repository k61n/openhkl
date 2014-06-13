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

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "InstrumentStore.h"
#include "Path.h"

namespace SX
{

namespace Instrument
{

using namespace boost::filesystem;
using namespace SX::Utils;

std::vector<std::string> InstrumentStore::_paths = {getInstrumentsPath(),getHomeDirectory()};

void InstrumentStore::addPath(const std::string& p, bool prepend)
{
	auto it=std::find(_paths.begin(),_paths.end(),p);
	if (it == _paths.end())
	{
		if (prepend)
			_paths.insert(_paths.begin(),p);
		else
			_paths.push_back(p);
	}
}

std::shared_ptr<Instrument> InstrumentStore::get(const std::string& key)
{
	for (auto it=_paths.begin();it!=_paths.end();++it)
	{
		path instrFile(*it);
		instrFile /= key + ".xml";
		if (exists(instrFile))
		{
			std::shared_ptr<Instrument> instr=std::shared_ptr<Instrument>(new Instrument());
			instr->load(instrFile.string());
			return instr;
		}
	}
	throw std::runtime_error("No instrument file found for "+key+" instrument.");
}

InstrumentStore::InstrumentStore()
{
}

} // End namespace Instrument

} // End namespace SX

