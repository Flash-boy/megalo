#include "../megalo/megalo.h"
#include <string>
#include <vector>

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void run_in_fiber(){
  MEGALO_LOG_INFO(g_logger) << "run_in_fiber begin";
  megalo::Fiber::YieldToHold();
  MEGALO_LOG_INFO(g_logger) << "run_in_fiber end";
  //megalo::Fiber::YieldToHold();
}
void test_fiber(){
  MEGALO_LOG_INFO(g_logger) << "main begin1";
  {
    MEGALO_LOG_INFO(g_logger) << "main begin";
    megalo::Fiber::GetThis();
    megalo::Fiber::ptr fiber(new megalo::Fiber(run_in_fiber));
    fiber->swapIn();
    MEGALO_LOG_INFO(g_logger) << "main after swapIn";
    fiber->swapIn();
    MEGALO_LOG_INFO(g_logger) << "main end";
  }
  MEGALO_LOG_INFO(g_logger) << "main end1";
}

int main(int argc, char** argv){
  megalo::Thread::SetName("main");
  std::vector<megalo::Thread::ptr> thrs;
  for(int i=0; i < 3; ++i){
    thrs.push_back(megalo::Thread::ptr(
          new megalo::Thread(&test_fiber, "name_" + std::to_string(i))));
  }
  for(auto i : thrs){
    i->join();
  }
  return 0;
}
