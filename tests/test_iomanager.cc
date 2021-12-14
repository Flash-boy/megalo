#include "../megalo/iomanager.h"
#include "../megalo/megalo.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

megalo::Logger::ptr g_logger = MEGALO_LOG_ROOT();
int sock = 0;

void test_fiber(){
  MEGALO_LOG_INFO(g_logger) << "test_fiber sock=" << sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  fcntl(sock, F_SETFL, O_NONBLOCK);

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(80);
  inet_pton(AF_INET, "", &addr.sin_addr.s_addr);

  if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))){
  }else if(errno == EINPROGRESS){
    MEGALO_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
    megalo::IOManager::GetThis()->addEvent(sock, megalo::IOManager::READ, [](){
      MEGALO_LOG_INFO(g_logger) << "read callback";     
    });
    megalo::IOManager::GetThis()->addEvent(sock, megalo::IOManager::WRITE, [](){
      MEGALO_LOG_INFO(g_logger) << "write callback";     

      megalo::IOManager::GetThis()->cancelEvent(sock, megalo::IOManager::READ);
      close(sock);
    });
  }else{
    MEGALO_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
  }
}

void test1(){
  megalo::IOManager iom(2, false);
  iom.scheduler(&test_fiber);
}

megalo::Timer::ptr s_timer;
void test_timer(){
  megalo::IOManager iom(2);
  s_timer = iom.addTimer(1000, [](){
      static int i = 0;
      MEGALO_LOG_INFO(g_logger) << "hello timer i=" << i;
      if(++i == 3){
        s_timer->reset(2000, true);
        s_timer->cancel();
      }
  }, true);
}

int main(int argc, char** argv){
  //test1();
  test_timer();
  return 0;
}
