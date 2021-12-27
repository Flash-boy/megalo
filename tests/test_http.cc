#include "../megalo/http/http.h"
#include "../megalo/log.h"

void test_request(){
  megalo::http::HttpRequest::ptr req(new megalo::http::HttpRequest);
  req->setHeader("host", "www.baidu.com");
  req->setBody("hello megalo");
  req->dump(std::cout) << std::endl;
}

void test_response(){
  megalo::http::HttpResponse::ptr rsp(new megalo::http::HttpResponse);
  rsp->setHeader("X-X", "megalo");
  rsp->setBody("hello megalo");
  rsp->setStatus(megalo::http::HttpStatus::NOT_FOUND);
  rsp->setClose(false);
  rsp->dump(std::cout) << std::endl;
}

int main(int argc, char** argv){
  test_request();
  test_response();
  return 0;
}
