#ifndef MEGALO_MACRO_H_
#define MEGALO_MACRO_H_

#include <assert.h>
#include <string.h>
#include "util.h"

#define MEGALO_ASSERT(x) \
  if(!(x)){ \
    MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "ASSERTION: " #x \
          << "\nbacktrace:\n" \
          << megalo::BacktraceToString(100, 2, "    "); \
    assert(x); \
  }

#define MEGALO_ASSERT2(x, w) \
  if(!(x)){ \
    MEGALO_LOG_ERROR(MEGALO_LOG_ROOT()) << "ASSERTION: " #x \
          << "\n" << w \
          << "\nbacktrace:\n" \
          << megalo::BacktraceToString(100, 2, "    "); \
    assert(x); \
  }



#endif
