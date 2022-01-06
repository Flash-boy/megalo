#include "../megalo/http/http_server.h"
#include "../megalo/log.h"

static megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

void run(){
  megalo::http::HttpServer::ptr server(new megalo::http::HttpServer);
  megalo::Address::ptr addr = megalo::Address::LookupAnyIpAddress("127.0.0.1:8020");
  while(!server->bind(addr)){
    sleep(2);
  }
  server->start();
}

int main(int argc, char** argv){
  megalo::IOManager iom(2);
  iom.schedule(run);
  return 0;
}
