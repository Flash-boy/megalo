#ifndef __MEGALO_HTTP_SERVLET_H_
#define __MEGALO_HTTP_SERVLET_H_

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include "http_session.h"
#include "http.h"
#include "../thread.h"

namespace megalo{
namespace http{

class Servlet{
public:
  typedef std::shared_ptr<Servlet> ptr;
  Servlet(const std::string& name)
    :m_name(name){
  }
  virtual ~Servlet(){}
  virtual int32_t handle(megalo::http::HttpRequest::ptr request
                        , megalo::http::HttpResponse::ptr response
                        , megalo::http::HttpSession::ptr session) = 0;
  const std::string& getName() { return m_name;}
protected:
  std::string m_name;
};

class FunctionServlet : public Servlet{
public:
  typedef std::shared_ptr<FunctionServlet> ptr;
  typedef std::function<int32_t (megalo::http::HttpRequest::ptr
                      , megalo::http::HttpResponse::ptr
                      , megalo::http::HttpSession::ptr)> callback;

  FunctionServlet(callback cb);
  virtual int32_t handle(megalo::http::HttpRequest::ptr request
                        , megalo::http::HttpResponse::ptr response
                        , megalo::http::HttpSession::ptr session) override;
private:
  callback m_cb;
};


class ServletDispatch : public Servlet{
public:
  typedef std::shared_ptr<ServletDispatch> ptr;
  typedef RWMutex RWMutexType;

  ServletDispatch();
  virtual int32_t handle(megalo::http::HttpRequest::ptr request
                        , megalo::http::HttpResponse::ptr response
                        , megalo::http::HttpSession::ptr session) override;

  void addServlet(const std::string& uri, Servlet::ptr slt);
  void addServlet(const std::string& uri, FunctionServlet::callback cb);
  void addGlobServlet(const std::string& uri, Servlet::ptr slt);
  void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

  void delServlet(const std::string& uri);
  void delGlobServlet(const std::string& uri);

  Servlet::ptr getDefault() { return m_default;}
  void setDefault(Servlet::ptr v) { m_default = v;}

  Servlet::ptr getServlet(const std::string& uri);
  Servlet::ptr getGlobServlet(const std::string& uri);

  Servlet::ptr getMatchServlet(const std::string& uri);
private:
  RWMutexType m_mutex;
  std::unordered_map<std::string, Servlet::ptr> m_datas;
  std::vector<std::pair<std::string, Servlet::ptr> > m_globs;
  Servlet::ptr m_default;
};

class NotFoundServlet : public Servlet{
public:
  typedef std::shared_ptr<NotFoundServlet> ptr;

  NotFoundServlet();
  virtual int32_t handle(megalo::http::HttpRequest::ptr request
                        , megalo::http::HttpResponse::ptr response
                        , megalo::http::HttpSession::ptr session) override;
};

}
}
#endif
