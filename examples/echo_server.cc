#include "../megalo/tcp_server.h"
#include "../megalo/log.h"
#include "../megalo/iomanager.h"
#include "../megalo/bytearray.h"
#include "../megalo/address.h"

static megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();

class EchoServer : public megalo::TcpServer {
public:
  EchoServer(int type);
  void handleClient(megalo::Socket::ptr client);

private:
  int m_type = 0;
};

EchoServer::EchoServer(int type)
  :m_type(type) {
}

void EchoServer::handleClient(megalo::Socket::ptr client) {
  MEGALO_LOG_INFO(g_logger) << "handleClient " << *client;   
  megalo::ByteArray::ptr ba(new megalo::ByteArray);
  while(true) {
    ba->clear();
    std::vector<iovec> iovs;
    ba->getWriteBuffers(iovs, 1024);

    int rt = client->recv(&iovs[0], iovs.size());
    if(rt == 0) {
      MEGALO_LOG_INFO(g_logger) << "client close: " << *client;
      break;
    } else if(rt < 0) {
      MEGALO_LOG_INFO(g_logger) << "client error rt=" << rt
        << " errno=" << errno << " errstr=" << strerror(errno);
      break;
    }
    ba->setPosition(ba->getPosition() + rt);
    ba->setPosition(0);
    if(m_type == 1) {//text 
      std::cout << ba->toString();// << std::endl;
    } else {
      std::cout << ba->toHexString();// << std::endl;
    }
    std::cout.flush();
  }
}

int type = 1;

void run() {
  MEGALO_LOG_INFO(g_logger) << "server type=" << type;
  EchoServer::ptr es(new EchoServer(type));
  auto addr = megalo::Address::LookupAny("0.0.0.0:8020");
  while(!es->bind(addr)) {
    sleep(2);
  }
  es->start();
}

int main(int argc, char** argv) {
  if(argc < 2) {
    MEGALO_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
    return 0;
  }

  if(!strcmp(argv[1], "-b")) {
    type = 2;
  }

  megalo::IOManager iom(2);
  iom.schedule(run);
  return 0;
}
