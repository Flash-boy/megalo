#ifndef __MEGALO_HTTP_HTTP_SESSION_H_
#define __MEGALO_HTTP_HTTP_SESSION_H_

#include "http.h"
#include "../socket_stream.h"

namespace megalo{
namespace http{
class HttpSession : public SocketStream {
public:
    typedef std::shared_ptr<HttpSession> ptr;

    HttpSession(Socket::ptr sock, bool owner = true);
    HttpRequest::ptr recvRequest();
    int sendResponse(HttpResponse::ptr rsp);
};

}
}

#endif
