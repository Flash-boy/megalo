#include "../megalo/socket.h"
#include "../megalo/log.h"
#include "../megalo/iomanager.h"
#include <string>  

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void test_socket(){
  megalo::IPAddress::ptr addr = megalo::Address::LookupAnyIpAddress("www.baidu.com");
  if(addr){
    MEGALO_LOG_INFO(g_logger) << "get address: " << addr->toString();
  }else{
    MEGALO_LOG_INFO(g_logger) << "get address fail";
    return;
  }
  megalo::Socket::ptr sock = megalo::Socket::CreateTCP(addr);
  addr->setPort(80);
  MEGALO_LOG_INFO(g_logger) << "addr=" << addr->toString();
  if(!sock->connect(addr)){
    MEGALO_LOG_INFO(g_logger) << "connect " << addr->toString() << " fail";
    return;
  }else{
    MEGALO_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
  }

  const char buff[] = "GET / HTTP/1.0\r\n\r\n";
  int rt = sock->send(buff, sizeof(buff));
  if(rt <= 0){
    MEGALO_LOG_INFO(g_logger) << "send fail rt=" << rt;
    return;
  }

  std::string buffs;
  buffs.resize(4096);
  rt = sock->recv(&buffs[0], buffs.size());
  if(rt <= 0){
    MEGALO_LOG_INFO(g_logger) << "recv fail rt=" << rt;
    return;
  }

  buffs.resize(rt);
  MEGALO_LOG_INFO(g_logger) << buffs;
}

int main(int argc, char** argv){
  test_socket();
  return 0;
}


