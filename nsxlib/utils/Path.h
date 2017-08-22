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

#pragma once

#include <string>

#include "../kernel/Singleton.h"

namespace nsx {

extern std::string g_path_separator;

extern int g_argc;

extern char** g_argv;

std::string trim(const std::string& input_path);

std::string removeFilename(const std::string& input_path);

//! Pass (argc, argv) or running process to nsxlib
void setArgv(int argc, char **argv);

//! Returns the home directory
std::string homeDirectory();

//! Returns the NSXTool application data path
std::string applicationDataPath();

//class Path : public Singleton<Path,Constructor,Destructor> {
//
//public:
//
//    //! Returns the path where the diffractometers XML definition files are stored
//    static std::string getDiffractometersPath();
//
//    static std::string getDataBasesPath(const std::string& database);
//
//    static std::string getResourcesDir();
//};

} // end namespace nsx
