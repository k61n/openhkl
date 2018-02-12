#include <cstdlib>
#include <fstream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>

#include "NSXConfig.h"
#include "Path.h"
#include "StringIO.h"

namespace nsx {

static std::string g_nsx_path("");

std::mutex g_nsx_path_mutex;

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

void setArgv(const char* nsx_path)
{
    std::lock_guard<std::mutex> guard(g_nsx_path_mutex);
    g_nsx_path = nsx_path;
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
    std::vector<std::string> possible_paths = {
        "",
        ".",
        "nsxtool",
        homeDirectory(),
        homeDirectory() + fileSeparator() + "nsxtool",
        g_application_data_path
    };

    // check for environment variable NSX_ROOT_DIR
    const char* nsx_root_dir = getenv("NSX_ROOT_DIR");

    // if defined, it takes highest precedence
    if (nsx_root_dir) {
        possible_paths.insert(possible_paths.begin(), nsx_root_dir);
    }

    // add base path of nsx library if possible
    if (!g_nsx_path.empty()) {
        std::string path = buildPath({g_nsx_path,"share","nsxtool"},"");
        possible_paths.insert(possible_paths.begin(), path);
    }

    std::vector<std::string> d19_relative_path = {"instruments","D19.yml"};

    for (auto&& path : possible_paths) {
        std::string d19_file = buildPath(d19_relative_path,path);
        std::ifstream file(d19_file, std::ios_base::in);
        if (file.good()) {
            file.close();
            return path;
        }
    }

    throw std::runtime_error("The application data directory could not be defined");
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

