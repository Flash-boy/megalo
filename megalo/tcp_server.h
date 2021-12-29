#ifndef __MEGALO_TCP_SERVER_H_
#define __MEGALO_TCP_SERVER_H_

#include <memory>
#include <functional>
#include <string>
#include "iomanager.h"
#include "address.h"
#include "socket.h"
#include "noncopyable.h"

namespace megalo{

class TcpServer : public std::enable_shared_from_this<TcpServer>
                  , Noncopyable{
public:
  typedef std::shared_ptr<TcpServer> ptr;

  TcpServer(megalo::IOManager* worker = megalo::IOManager::GetThis()
           ,megalo::IOManager* accept_worker = megalo::IOManager::GetThis());
  virtual ~TcpServer();
  virtual bool bind(megalo::Address::ptr addr);
  virtual bool bind(const std::vector<Address::ptr>& addrs
                    , std::vector<Address::ptr>& fails);
  virtual bool start();
  virtual void stop();
  uint64_t getRecvTimeout() const { return m_recvTimeout;}
  std::string getName() const { return m_name;}
  void setTimeout(uint64_t v) { m_recvTimeout = v;}
  virtual void setName(const std::string& v) { m_name = v;}
  bool isStop() const { return m_isStop;}
  virtual std::string toString(const std::string& prefix = "");
protected:
  virtual void handleClient(Socket::ptr client);
  virtual void startAccept(Socket::ptr sock);
private:
  std::vector<Socket::ptr> m_socks;
  IOManager* m_worker;
  IOManager* m_acceptWorker;
  uint64_t m_recvTimeout;
  std::string m_name;
  std::string m_type = "tcp";
  bool m_isStop;
};

}
#endif
