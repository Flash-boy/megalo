#ifndef __MEGALO_FDMANAGER_H_
#define __MEGALO_FDMANAGER_H_

#include <memory>
#include <vector>
#include "thread.h"
#include "singleton.h"

namespace megalo{

// 管理文件句柄上下文类,是否是socket
// 是否阻塞，是否关闭，读写超时时间
class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
  typedef std::shared_ptr<FdCtx> ptr;

  FdCtx(int fd);
  ~FdCtx();

  bool isInit() const { return m_isInit;}
  bool isSocket() const { return m_isSocket;}
  bool isClosed() const { return m_isClosed;}

  void setUserNonblock(bool v){ m_userNonblock = v;}
  bool getUserNonblock() const { return m_userNonblock;}
  void setSysNonblock(bool v){ m_sysNonblock = v;};
  bool getSysNonblock() const { return m_sysNonblock;}

  // 设置超时时间
  // type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
  void setTimeout(int type, uint64_t v);

  // 获取超时时间
  // type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
  uint64_t getTimeout(int type);
private:
  bool init();
private:
  bool m_isInit: 1;
  bool m_isSocket: 1;
  bool m_sysNonblock: 1;
  bool m_userNonblock: 1;
  bool m_isClosed: 1;
  int m_fd;
  uint64_t m_recvTimeout;
  uint64_t m_sendTimeout;
};


class FdManager{ public:
  typedef RWMutex RWMutexType;

  FdManager();
  ~FdManager();
  FdCtx::ptr get(int fd, bool auto_created = false);
  void del(int fd);
private:
  RWMutexType m_mutex;
  std::vector<FdCtx::ptr> m_datas;
};

// 文件句柄单例
typedef Singleton<FdManager> FdMgr;

}
#endif
