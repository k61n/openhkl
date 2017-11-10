#include <cstdlib>
#include <fstream>
#include <mutex>
#include <numeric>
#include <stdexcept>

#include "NSXConfig.h"
#include "Path.h"
#include "StringIO.h"

namespace nsx {

#ifdef _WIN32
    static const std::string g_path_separator = "\\";
#else
    static const std::string g_path_separator = "/";
#endif

static int g_argc = 0;

static char** g_argv = nullptr;

std::mutex g_argc_mutex;
std::mutex g_argv_mutex;

std::string dirname(const std::string& input_path)
{

    std::string output_path = trimmed(input_path);

    output_path.erase(output_path.find_last_of(g_path_separator),output_path.size()-1);

    return output_path;
}

void setArgc(int argc)
{
    std::lock_guard<std::mutex> guard(g_argc_mutex);
    g_argc = argc;
}

void setArgv(char **argv)
{
    std::lock_guard<std::mutex> guard(g_argv_mutex);
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

std::string buildPath(const std::string& root, const std::vector<std::string>& paths)
{
    auto append_path = [](std::string base, std::string p){return base+g_path_separator+p;};

    std::string path = std::accumulate(paths.begin(),paths.end(),root,append_path);

    return path;
}

std::string applicationDataPath()
{
    std::vector<std::string> possible_paths = {
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
        possible_paths.insert(possible_paths.begin(), nsx_root_dir);
    }

    // add location of executable if possible
    if (g_argc > 0 && g_argv && g_argv[0]) {
        std::string path = dirname(g_argv[0]);
        possible_paths.insert(possible_paths.begin(), path);
    }

    std::vector<std::string> d19_relative_path = {"instruments","D19.yml"};

    for (auto&& path : possible_paths) {
        std::string d19_file = buildPath(path,d19_relative_path);

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
    std::string path = applicationDataPath() + g_path_separator + "instruments";
    return path;
}

} // end namespace nsx

