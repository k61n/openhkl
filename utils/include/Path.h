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

#ifndef NSXTOOL_PATH_H_
#define NSXTOOL_PATH_H_

#include <string>
#include <unistd.h>

#include <boost/filesystem.hpp>

namespace SX
{

namespace Utils
{

using namespace boost::filesystem;

std::string getInstallationPath()
{
	char* ppath;
	ppath = getenv("NSXTOOL");
	if (ppath!=NULL)
		return std::string(ppath);
	else
		return "";
}

std::string getResourcesPath()
{
	path p(getInstallationPath());
	p /= "resources";
	return p.string();
}

std::string expandUser(std::string path)
{
	// the path must start with ~ to be user expanded.
	if (not path.empty() and path[0] == '~')
	{
		char const* home = getenv("HOME");
		if (home or ((home = getenv("USERPROFILE"))))
			path.replace(0, 1, home);
		else
		{
			char const *hdrive = getenv("HOMEDRIVE"),
			*hpath = getenv("HOMEPATH");
			assert(hdrive);
			assert(hpath);
			path.replace(0, 1, std::string(hdrive) + hpath);
		}
		assert(portable_name(path));
	}
	return path;
}

} // end namespace Utils

} // end namespace SX

#endif
