#include "http_server.h"
#include "../log.h"

namespace megalo{
namespace http{
  
static megalo::Logger::ptr g_logger = MEGALO_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive
    ,megalo::IOManager* worker
    ,megalo::IOManager* accept_worker)
  :TcpServer(worker, accept_worker)
  ,m_isKeepalive(keepalive){
  m_dispatch.reset(new ServletDispatch);
}

void HttpServer::handleClient(Socket::ptr client){
  HttpSession::ptr session(new HttpSession(client));
  do{
    auto req = session->recvRequest();
    if(!req){
      MEGALO_LOG_WARN(g_logger) << "recv http request fail, errno="
          << errno << " errstr=" << strerror(errno)
          << " client" << *client;
      break;
    }
    HttpResponse::ptr rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepalive));

    m_dispatch->handle(req, rsp, session);
    session->sendResponse(rsp);
  }while(m_isKeepalive);
  session->close();
}

}
}
