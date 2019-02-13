#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>

#include "Path.h"
#include "StringIO.h"

namespace nsx {

std::string fileSeparator()
{
    #ifdef _WIN32
        return "\\";
    #else
        return "/";
    #endif
}

std::string fileBasename(const std::string& input_path)
{
    std::string output_path = trim(input_path);

    auto pos = output_path.find_last_of(fileSeparator());

    if (pos != std::string::npos) {
        output_path.erase(0,pos+1);
    }
    return output_path;
}

std::pair<std::string,std::string> splitFileExtension(const std::string& input_path)
{
    std::pair<std::string,std::string> output;

    output.first = trim(input_path);

    auto pos = output.first.find_last_of(".");
    if (pos != std::string::npos) {
        output.second = output.first.substr(pos);
        output.first.erase(pos);
    } else {
        output.second = "";
    }

    return output;
}


std::string fileDirname(const std::string& input_path)
{
    std::string output_path = trim(input_path);

    auto pos = output_path.find_last_of(fileSeparator());

    if (pos != std::string::npos) {
        output_path.erase(pos,output_path.size()-1);
    }

    return output_path;
}

void makeDirectory(const std::string& path, int mode)
{

    auto components = split(path,fileSeparator());

    std::vector<std::string> intermediate_paths;
    intermediate_paths.reserve(components.size());

    for (auto comp : components) {
        intermediate_paths.push_back(comp);
        auto inner_path = join(intermediate_paths,fileSeparator());
        #if defined(_WIN32)
            _mkdir(inner_path.c_str());
        #else
            mkdir(inner_path.c_str(),mode);
        #endif
    }

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
                home += fileSeparator();
                home += hpath;
                return home;
            }
        }
    }
    // Otherwise throw and error
    throw std::runtime_error("The home directory could not be defined");
}

std::string buildPath(const std::vector<std::string>& paths, const std::string& root)
{
    std::string path = join(paths,fileSeparator());

    if (root.empty()) {
        return path;
    } else {
        return root + fileSeparator() + path;
    }
}

std::string applicationDataPath()
{
    std::string appdata_path;
    #ifdef _WIN32
    // check for environment variable APPDATA
    const char* appdata_dir = getenv("APPDATA");
    // if defined, it takes highest precedence
    if (appdata_dir) {
        appdata_path = buildPath({appdata_dir,"nsxtool","resources"},"");
        return appdata_path;
    }
    #endif

    appdata_path = buildPath({homeDirectory(),".nsxtool","resources"},"");

    return appdata_path;
}

std::string diffractometersPath()
{
    std::string path = applicationDataPath() + fileSeparator() + "instruments";
    return path;
}

bool fileExists(const std::string& filename)
{
    std::ifstream fs(filename);
    return fs.good();
}

} // end namespace nsx

