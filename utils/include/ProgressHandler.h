#ifndef PROGRESSHANDLER_H
#define PROGRESSHANDLER_H

#include <functional>

namespace SX {

    namespace Utils {

        // progress handler callback:
        // progressHandler(double progress) -> int
        // argument progress is the progress (as a percentage)
        using ProgressHandler = std::function<void(double)>;

    } // namespace Utils

} // namespace SX


#endif // PROGRESSHANDLER_H
