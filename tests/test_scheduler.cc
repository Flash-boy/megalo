#include "../megalo/megalo.h"

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();


void test_fiber(){
  static int s_count = 2;
  MEGALO_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;

  sleep(1);
  if(--s_count >= 0){
    megalo::Scheduler::GetThis()->schedule(&test_fiber, megalo::GetThreadId());
  }
}

int main(int argc, char** argv){
  megalo::Thread::SetName("main");
  MEGALO_LOG_INFO(g_logger) << "main";  
  megalo::Scheduler sc(2, true, "test");
  sc.start();
  sleep(2);
  MEGALO_LOG_INFO(g_logger) << "scheduler";
  //sc.scheduler(&test_fiber);
  sc.stop();
  MEGALO_LOG_INFO(g_logger) << "over";
  return 0;
}
