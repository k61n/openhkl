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

#include "Singleton.h"

namespace SX
{

namespace Utils
{

class Path : public SX::Kernel::Singleton<Path,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

	//! Expands a given path with the HOME directory. The input path must start with ~ other wise it is returned unchanged
	static std::string expandUser(std::string path);

	//! Returns the path to HOME
	static std::string getHomeDirectory();

	//! Returns the NSXTool application data path
	static std::string getApplicationDataPath();

	//! Returns the path where the diffractomers XML defininition files are stored
	static std::string getDiffractometersPath();

	static std::string getDataBasesPath();

    static std::string getResourcesDir();

    //! Pass (argc, argv) or running process to nsxlib
    static void setArgv(int argc, char** argv);

private:
    static int _argc;
    static char** _argv;
};


} // end namespace Utils

} // end namespace SX

#endif
