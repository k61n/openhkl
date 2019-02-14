/*********************************************************************
  Blosc - Blocked Suffling and Compression Library

  Unit tests for basic features in Blosc.

  Creation date: 2010-06-07
  Author: Francesc Alted <francesc@blosc.org>

  See LICENSES/BLOSC.txt for details about copyright and rights to use.
**********************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
#include "win32/stdint-windows.h"
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include "../blosc/blosc.h"
#include <math.h>

/* This is MinUnit in action (http://www.jera.com/techinfo/jtns/jtn002.html) */
#define mu_assert(message, test)                                               \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)
#define mu_run_test(test)                                                      \
  do {                                                                         \
    char *message = test();                                                    \
    tests_run++;                                                               \
    if (message) {                                                             \
      printf("%c", 'F');                                                       \
      return message;                                                          \
    } else                                                                     \
      printf("%c", '.');                                                       \
  } while (0)

extern int tests_run;

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)
