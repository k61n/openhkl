#include <cstdlib>

#include <boost/filesystem.hpp>

#include "Config.h"
#include "Error.h"
#include "Path.h"

namespace SX
{

namespace Utils
{

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
#ifdef __linux
  // TODO: application data path should NOT be hard coded
	boost::filesystem::path p("/usr/local/share");
	p /= "nsxtool";
	return p.string();
#elif defined (__APPLE__)
	// TODO: application data path should NOT be hard coded
    boost::filesystem::path p("/usr/local/share");
    p /= "nsxtool";
    return p.string();
#elif defined (_WIN32)
    // TODO: implement this correctly for windows
    boost::filesystem::path p("");
    p /= "nsxtool";
    return p.string();
#endif
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


} // end namespace Utils

} // end namespace SX
