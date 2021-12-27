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

std::string ToUpper(const std::string& name);
std::string ToLower(const std::string& name);
std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

class StringUtil{
public:
  static std::string Format(const char* fmt, ...);
  static std::string Formatv(const char* fmt, va_list ap);
  
  static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
  static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

  static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
  static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
  static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");

  static std::string WStringToString(const std::wstring& ws);
  static std::wstring StringToWString(const std::string& s);

};

}
#endif
