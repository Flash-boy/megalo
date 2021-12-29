#include "../megalo/tcp_server.h"
#include "../megalo/iomanager.h"
#include "../megalo/log.h"
#include <vector>

static megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void run(){
  auto addr = megalo::Address::LookupAny("127.0.0.1:8033");
  //auto addr2 = megalo::UnixAddress::ptr(new megalo::UnixAddress("/tmp/unix_addr"));
  std::vector<megalo::Address::ptr> addrs;
  addrs.push_back(addr);
  //addrs.push_back(addr2);

  megalo::TcpServer::ptr tcp_server(new megalo::TcpServer);
  std::vector<megalo::Address::ptr> fails;
  while(!tcp_server->bind(addrs, fails)){
    sleep(2);
  }
  tcp_server->start();
}

int main(int argc, char** agrv){
  megalo::IOManager iom(2);
  iom.schedule(run);
  return 0;
}
