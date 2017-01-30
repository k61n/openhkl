#include <cstdlib>

#include <boost/filesystem.hpp>
#include <fstream>


#include "NSXConfig.h"
#include "Error.h"
#include "Path.h"

namespace SX
{

namespace Utils
{

int Path::_argc = 0;
char** Path::_argv = nullptr;

std::string Path::getHomeDirectory()
{
    const char* home = getenv("HOME");
    // Build the home directory from HOME environment variable
    if (home)
        return std::string(home);
    // If HOME is not defined (on Windows it may happen) define the home directory from USERPROFILE environment variable
    else
    {
        home = getenv("USERPROFILE");
        if (home)
            return std::string(home);
        // If the USERPROFILE environment variable is not defined try to build a home directory from the HOMEDRIVE and HOMEPATH environment variable
        else
        {
            char const *hdrive = getenv("HOMEDRIVE");
            char const *hpath = getenv("HOMEPATH");
            if (hdrive && hpath)
            {
                boost::filesystem::path p(hdrive);
                p/=hpath;
                return p.string();
            }

        }
    }
    // Otherwise throw and error
    throw SX::Kernel::Error<Path>("The home directory could not be defined");
}

std::string Path::expandUser(std::string path)
{
    // the path must start with ~ to be user expanded.
    if (!path.empty() && path[0] == '~')
    {
        std::string home(getHomeDirectory());
        path.replace(0, 1, home);
    }
    return path;
}

std::string Path::getApplicationDataPath()
{
    std::vector<std::string> possible_locations = {
        "",
        ".",
        "nsxtool",
        getHomeDirectory(),
        getHomeDirectory() + "/nsxtool",
        g_resourcesDir,
        "/usr/share/nsxtool",
        "/usr/local/share/nsxtool"
    };

    // check for environment variable NSX_ROOT_DIR
    const char* nsx_root_dir = getenv("NSX_ROOT_DIR");

    // if defined, it takes highest precedence
    if ( nsx_root_dir)
        possible_locations.insert(possible_locations.begin(), nsx_root_dir);

    // add location of executable if possible
    if ( _argc > 0 && _argv && _argv[0]) {
        boost::filesystem::path p(_argv[0]);
        p.remove_filename();
        possible_locations.insert(possible_locations.begin(), p.string());
    }

    std::string match = "";

    for (auto&& path: possible_locations) {
        std::string path_str = boost::filesystem::path(path + "/instruments/D19.xml").string();

        std::ifstream file(path_str, std::ios_base::in);
        if ( file.good() ) {
            match = path;
            break;
        }
    }

    // did not find a match
    if ( match == "" )
        throw SX::Kernel::Error<Path>("The application data directory could not be defined");

    return boost::filesystem::path(match).string();
}


std::string Path::getDiffractometersPath()
{
    boost::filesystem::path p(getResourcesDir());
    p /= "instruments";
    return p.string();
}

std::string Path::getDataBasesPath()
{
    boost::filesystem::path p(getResourcesDir());
    p /= "databases";
    return p.string();
}

std::string Path::getResourcesDir()
{
    static std::string resourcesDir;

    if ( resourcesDir == "")
        resourcesDir = getApplicationDataPath();

    return boost::filesystem::path(resourcesDir).string();
}

void Path::setArgv(int argc, char **argv)
{
    _argc = argc;
    _argv = argv;
}




} // end namespace Utils

} // end namespace SX
