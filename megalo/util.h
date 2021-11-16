#ifndef _MEGALO_UTIL_H_
#define _MEGALO_UTIL_H_

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>

namespace megalo{

pid_t GetThreadId();
uint32_t GetFiberId();

}
#endif
