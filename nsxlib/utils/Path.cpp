#include <cstdlib>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <iostream>

#include <boost/filesystem.hpp>

#include "Path.h"

#include "NSXConfig.h"

namespace nsx {

//#ifdef _WIN32
//    extern const std::string g_path_separator = "\\";
//#else
//    extern const std::string g_path_separator = "/";
//#endif

std::string g_path_separator = "toto";

int g_argc = 100;

char** g_argv = nullptr;

std::string trim(const std::string& input_path) {

    std::string output_path(input_path);
    output_path.erase(output_path.find_last_not_of(" \n\r\t")+1);
    output_path.erase(0,output_path.find_first_not_of(" \n\r\t"));

    return output_path;
}

std::string removeFilename(const std::string& input_path) {

    std::string output_path = trim(input_path);

    output_path.erase(output_path.find_last_of(g_path_separator),output_path.size()-1);

    return output_path;
}

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

std::string applicationDataPath()
{
    std::vector<std::string> possible_locations = {
        "",
        ".",
        "nsxtool",
        homeDirectory(),
        homeDirectory() + g_path_separator + "nsxtool",
        g_application_data_path
    };

    // check for environment variable NSX_ROOT_DIR
    const char* nsx_root_dir = getenv("NSX_ROOT_DIR");

    // if defined, it takes highest precedence
    if (nsx_root_dir) {
        possible_locations.insert(possible_locations.begin(), nsx_root_dir);
    }

    // add location of executable if possible
    if (g_argc > 0 && g_argv && g_argv[0]) {
        std::string path = removeFilename(g_argv[0]);
        possible_locations.insert(possible_locations.begin(), path);
    }

    auto append_path = [](std::string base, std::string p){return base+g_path_separator+p;};

    std::vector<std::string> d19_path = {"instruments","D19.xml"};

    for (auto&& possible_path : possible_locations) {

    for (auto&& path: possible_locations) {
        std::cout<<"path_sep"<<g_path_separator<<std::endl;
        std::cout<<"argc"<<g_argc<<std::endl;

        std::string path = std::accumulate(d19_path.begin(),d19_path.end(),possible_path,append_path);

        std::cout<<"dsadsad"<<path<<std::endl;

        std::ifstream file(path, std::ios_base::in);
        if (file.good()) {
            file.close();
            return removeFilename(path);
        }
    }

    throw std::runtime_error("The application data directory could not be defined");
}

//std::string Path::getDiffractometersPath()
//{
//    boost::filesystem::path p(getResourcesDir());
//    p /= "instruments";
//    return p.string();
//}
//
//std::string Path::getDataBasesPath(const std::string& database)
//{
//    boost::filesystem::path p(getResourcesDir());
//    p /= "databases";
//    p /= database;
//    return p.string();
//}
//
//std::string Path::getResourcesDir()
//{
//    static std::string resourcesDir;
//
//    if ( resourcesDir == "") {
//        resourcesDir = applicationDataPath();
//    }
//    return boost::filesystem::path(resourcesDir).string();
//}

} // end namespace nsx

