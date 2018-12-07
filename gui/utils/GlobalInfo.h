#pragma once

#include <QSysInfo>

enum class OSType { WINDOWS_OS, LINUX_OS, MAC_OS, OTHER_UNIX_OS, OTHER_OS };

OSType hostOS()
{
#if defined(Q_OS_WIN)
    return OSType::WINDOWS_OS;
#elif defined(Q_OS_LINUX)
    return OSType::LINUX_OS;
#elif defined(Q_OS_MAC)
    return OSType::MAC_OS;
#elif defined(Q_OS_UNIX)
    return OSType::OTHER_UNIX_OS;
#else
    return OSType::OTHER_OS;
#endif
}
