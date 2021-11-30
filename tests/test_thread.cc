#include "../megalo/megalo.h"
#include <string>
#include <vector>

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

megalo::Mutex g_mutex;

int count = 0;

void func1(){
  MEGALO_LOG_INFO(g_logger) << "name: " << megalo::Thread::GetName() 
                            << " this.name " << megalo::Thread::GetThis()->getName() 
                            << " id: " << megalo::GetThreadId() 
                            << " this.id: " << megalo::Thread::GetThis()->getId();
  for(int i=0; i < 10000000; ++i){
    megalo::Mutex::Lock lock(g_mutex);
    ++count;
  }
}
void func2(){
  while(true){
    MEGALO_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  }
}
void func3(){
  while(true){
    MEGALO_LOG_INFO(g_logger) << "=================================================";
  }
}

int main(){
  MEGALO_LOG_INFO(g_logger) << "thread test begin";
  YAML::Node root = YAML::LoadFile("/home/mars/Project/megalo/bin/conf/log2.yml");
  megalo::Config::LoadFromYaml(root);

  std::vector<megalo::Thread::ptr> threads;
  for(size_t i = 0; i < 2; ++i){
    megalo::Thread::ptr thr2(new megalo::Thread(&func2, "name_" + std::to_string(i*2)));
    megalo::Thread::ptr thr3(new megalo::Thread(&func3, "name_" + std::to_string(i*2+1)));
    threads.push_back(thr2);
    threads.push_back(thr3);
  }

  for(size_t i=0; i < threads.size(); ++i){
    threads[i]->join();
  }

  return 0;

}

