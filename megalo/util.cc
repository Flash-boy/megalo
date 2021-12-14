#include "util.h"
#include "log.h"
#include "fiber.h"
#include <sstream>
#include <string>
#include <sys/time.h>

namespace megalo{

static megalo::Logger::ptr g_logger = MEGALO_LOG_NAME("system");

pid_t GetThreadId(){
  return syscall(SYS_gettid);
}

uint32_t GetFiberId(){
  return megalo::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
  void** array = (void**)malloc(sizeof(void*) * size);
  size_t s = ::backtrace(array, size);
  char** strings = backtrace_symbols(array, s); 
  if(strings == NULL){
    MEGALO_LOG_ERROR(g_logger) << "backtrace_symbols error";  
    free(array);
    return;
  }

  for(size_t i = skip; i < s; ++i){
    bt.push_back(strings[i]);
  }
  free(array);
  free(strings);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
  std::vector<std::string> bt;
  Backtrace(bt, size, skip);
  std::stringstream ss;
  for(size_t i = 0; i < bt.size(); ++i){
    ss << prefix << bt[i] << "\n";
  }
  return ss.str();
}

uint64_t GetCurrentMS(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}
uint64_t GetCurrentUS(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}


}

