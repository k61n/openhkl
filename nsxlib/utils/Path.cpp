#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "NSXConfig.h"
#include "../utils/Path.h"

namespace nsx {

#ifdef _WIN32
    extern const std::string std::string g_path_separator = "\\";
#else
    extern const std::string g_path_separator = "/";
#endif

int g_argc = 0;
char** g_argv = nullptr;

void setArgv(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;
}

std::string homeDirectory()
{
    const char* home_envvar = getenv("HOME");

    // Build the home directory from HOME environment variable
    if (home_envvar) {
        return std::string(home_envvar);
    }
    // If HOME is not defined (on Windows it may happen) define the home
    // directory from USERPROFILE environment variable
    else {
        home_envvar = getenv("USERPROFILE");
        if (home_envvar)
            return std::string(home_envvar);
        // If the USERPROFILE environment variable is not defined try to build
        // a home directory from the HOMEDRIVE and HOMEPATH environment variable
        else {
            char const *hdrive = getenv("HOMEDRIVE");
            char const *hpath = getenv("HOMEPATH");
            if (hdrive && hpath) {
                std::string home(hdrive);
                home += g_path_separator;
                home += hpath;
                return home;
            }
        }
    }
    // Otherwise throw and error
    throw std::runtime_error("The home directory could not be defined");
}

std::string Path::getApplicationDataPath()
{
    std::vector<std::string> possible_locations = {
        "",
        ".",
        "nsxtool",
        homeDirectory(),
        homeDirectory() + "/nsxtool",
        g_resourcesDir,
        "/usr/share/nsxtool",
        "/usr/local/share/nsxtool"
    };

    // check for environment variable NSX_ROOT_DIR
    const char* nsx_root_dir = getenv("NSX_ROOT_DIR");

    // if defined, it takes highest precedence
    if ( nsx_root_dir) {
        possible_locations.insert(possible_locations.begin(), nsx_root_dir);
    }
    // add location of executable if possible
    if (g_argc > 0 && g_argv && g_argv[0]) {
        boost::filesystem::path p(g_argv[0]);
        p.remove_filename();
        possible_locations.insert(possible_locations.begin(), p.string());
    }

    std::string match = "";

    for (auto&& path: possible_locations) {
        std::string path_str = boost::filesystem::path(path + "/instruments/D19.yaml").string();

        std::ifstream file(path_str, std::ios_base::in);
        if ( file.good() ) {
            match = path;
            break;
        }
    }

    // did not find a match
    if ( match == "" ) {
        throw std::runtime_error("The application data directory could not be defined");
    }
    return boost::filesystem::path(match).string();
}


std::string Path::getDiffractometersPath()
{
    boost::filesystem::path p(getResourcesDir());
    p /= "instruments";
    return p.string();
}

std::string Path::getDataBasesPath(const std::string& database)
{
    boost::filesystem::path p(getResourcesDir());
    p /= "databases";
    p /= database;
    return p.string();
}

std::string Path::getResourcesDir()
{
    static std::string resourcesDir;

    if ( resourcesDir == "") {
        resourcesDir = getApplicationDataPath();
    }
    return boost::filesystem::path(resourcesDir).string();
}

void Path::setArgv(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;
}

} // end namespace nsx

