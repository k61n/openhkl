//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      InstrumentParameters.h
//! @brief     Declares functions that relate to resource files
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_INSTRUMENTPARAMETERS_H
#define OHKL_CORE_INSTRUMENT_INSTRUMENTPARAMETERS_H

#include <yaml-cpp/yaml.h>

#include <map>
#include <stdexcept>

#ifdef __APPLE__
#include <mach-o/dyld.h>

inline std::string applicationDirPath()
{
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
    {
        char *real = realpath(path, nullptr);  // resolves symlinks
        std::string const fullPath = real;
        free(real);
        return fullPath.substr(0, fullPath.find_last_of('/'));
    }
    return "";
}
#elif defined(_WIN32)
#include <windows.h>

inline std::string applicationDirPath()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath = path;
    return fullPath.substr(0, fullPath.find_last_of('\\'));
}
#endif

namespace ohkl {

namespace Instrument {

//! Map of instrument names {name, description}
static const std::map<std::string, std::string> instrument_list = {
    {"BioDiff", "BioDiff"},
    {"POLI", "POLI"},
    {"HEIDI", "HEIDI"},
    {"D9", "D9"},
    {"D9_lifting_arm", "D9 lifting arm"},
    {"D9_large", "D9 large"},
    {"D9_large_lifting_arm", "D9 large lifting arm"},
    {"D10", "D10"},
    {"D19", "D19"},
    {"I16", "I16"},
};

//! Return a YAML::Node with the detector information
inline YAML::Node findResource(const std::string& instrumentName)
{
#ifdef __linux__
    const char* CI_PROJECT_DIR = std::getenv("CI_PROJECT_DIR");
#endif
    std::string const resource_dir =
#ifdef __linux__
        (CI_PROJECT_DIR) ? std::string(CI_PROJECT_DIR) + "/data/instruments" : std::string(DATAROOTDIR) + "/openhkl/instruments";
#elif defined(__APPLE__)
        applicationDirPath() + "/../Resources/instruments";
#elif defined(_WIN32)
        applicationDirPath() + "/instruments";
#endif
    if (!instrument_list.count(instrumentName))
        throw std::runtime_error("findResource: unable to find instrument " + instrumentName);
    std::string path = resource_dir + "/" + instrumentName + ".yml";
    return YAML::LoadFile(path);
}

}// namespace Instrument

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_INSTRUMENTPARAMETERS_H
