#ifndef __MEGALO_HTTP_HTTP_SERVER_H_
#define __MEGALO_HTTP_HTTP_SERVER_H_

#include "http_session.h"
#include "../tcp_server.h"
#include "servlet.h"

namespace megalo{
namespace http{

class HttpServer : public TcpServer {
public:
  typedef std::shared_ptr<HttpServer> ptr;
  HttpServer(bool keepalive = false
      ,megalo::IOManager* worker = megalo::IOManager::GetThis()
      ,megalo::IOManager* accept_worker = megalo::IOManager::GetThis());

  ServletDispatch::ptr getServletDispatch() const { return m_dispatch;}
  void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v;}
protected:
  virtual void handleClient(Socket::ptr client) override;
private:
  /// 是否支持长连接
  bool m_isKeepalive;
  ServletDispatch::ptr m_dispatch;    
};

}
}

#endif
