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
#include <vector>

#include "../kernel/Singleton.h"

namespace nsx {

// left and right trim a given path
std::string trim(const std::string& input_path);

//! Get the directory name of a given path
std::string dirname(const std::string& input_path);

std::string buildPath(const std::string& root, const std::vector<std::string>& paths);

//! Returns the home directory
std::string homeDirectory();

//! Returns the NSXTool application data path
std::string applicationDataPath();

//! Pass argc of running process to nsxlib
void setArgc(int argc);

//! Set argv of running process to nsxlib
void setArgv(char** argv);

std::string diffractometersPath();

} // end namespace nsx
