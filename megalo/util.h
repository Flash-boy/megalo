#ifndef _MEGALO_UTIL_H_
#define _MEGALO_UTIL_H_

#include <pthread.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <execinfo.h>

namespace megalo{

// 返回线程Id
pid_t GetThreadId();
// 返回协程Id
uint32_t GetFiberId();

// 获取当前的调用栈帧
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

// 获取当前调用栈帧字符串信息
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

// 获取当前时间 ms
uint64_t GetCurrentMS();
// 获取当前时间 us
uint64_t GetCurrentUS();

}
#endif
