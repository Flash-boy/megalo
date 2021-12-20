#include "../megalo/address.h"
#include "../megalo/log.h"
#include <vector>

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void test(){
  std::vector<megalo::Address::ptr> addrs;

  MEGALO_LOG_INFO(g_logger) << "begin";
  //bool v = megalo::Address::Lookup(addrs, "localhost:3080");
  bool v = megalo::Address::Lookup(addrs, "www.baidu.com");
  MEGALO_LOG_INFO(g_logger) << "end";
  if(!v){
    MEGALO_LOG_ERROR(g_logger) << "lookup fail";
    return;
  }

  for(size_t i = 0; i <addrs.size(); ++i){
    MEGALO_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
  }

  auto addr = megalo::Address::LookupAny("localhost:4080");
  if(addr){
    MEGALO_LOG_INFO(g_logger) << *addr;
  }else{
    MEGALO_LOG_ERROR(g_logger) << "error";
  }
}

void test_iface(){
  std::multimap<std::string, std::pair<megalo::Address::ptr, uint32_t> > results;

  bool v = megalo::Address::GetInterfaceAddresses(results);
  if(!v) {
    MEGALO_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
    return;
  }

  for(auto& i: results) {
    MEGALO_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - "
      << i.second.second;
  }
}

void test_ipv4(){
  auto addr = megalo::IPAddress::Create("127.0.0.8");
    if(addr) {
        MEGALO_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char** argv){
  //test_ipv4();
  //test_iface();
  test();
}

