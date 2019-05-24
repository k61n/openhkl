#ifndef CORE_UTILS_SYSTEM_H
#define CORE_UTILS_SYSTEM_H

// Define necessary symbols for exporting symbols in dll table.
#ifdef _WIN32
#ifdef core_EXPORT
#define DLLExport __declspec(dllexport)
#else
#define DLLEXport __declspec(dllimport)
#endif
#else
#define DLLExport
#endif

#endif // CORE_UTILS_SYSTEM_H
