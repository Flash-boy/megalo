#ifndef __MEGALO_IOMANAGER_H_
#define __MEGALO_IOMANAGER_H_

#include "scheduler.h"
#include "timer.h"
#include <memory>
#include <vector>

namespace megalo{

class IOManager : public Scheduler, public TimerManager{
public:
  typedef std::shared_ptr<IOManager> ptr;
  typedef RWMutex RWMutexType;
  // IO事件
  enum Event {
    // 无事件
    NONE = 0x0,
    // 读事件(EPOLL_IN)
    READ = 0x01,
    // 写事件(EPOLL_OUT)
    WRITE = 0x04
  };

private:
  // Socket事件上下文类
  struct FdContext{
    typedef Mutex MutexType;
    // 事件上下文类
    struct EventContext{
      Scheduler* scheduler = nullptr;
      Fiber::ptr fiber;
      std::function<void()> cb;
    };
    // 获取事件山下文
    EventContext& getContext(Event event);
    // 重置事件上下文
    void resetContext(EventContext& ctx);
    // 触发事件
    void triggerEvent(Event event);

    public:  
    // 读事件上下文
    EventContext read;
    // 写事件上下文
    EventContext write;
    // 事件关联的句柄
    int fd = 0;
    // 当前事件
    Event events = NONE;
    MutexType mutex;
  };

public:
  IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
  ~IOManager();

  // 添加事件
  // 成功返回0,失败返回-1
  int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
  // 删除事件
  bool delEvent(int fd, Event event);
  // 取消事件,如果事件存在则触发事件
  bool cancelEvent(int fd, Event event);
  // 取消所有事件
  bool cancelAll(int fd);
  static IOManager* GetThis();

protected:
  void tickle() override;
  bool stopping() override;
  void idle() override;
  void onTimerInsertAtFront() override;
  // 重置socket句柄上下文容器大小
  void contextResize(size_t size);
  
  bool stopping(uint64_t& timeout);

private:
  // epoll文件句柄
  int m_epfd = 0;
  // pipe文件句柄
  int m_tickleFds[2];
  // 当前等待执行事件数量 
  std::atomic<size_t> m_pendingEventCount = {0};
  RWMutexType m_mutex;
  // socket事件上下文容器
  std::vector<FdContext*> m_fdContexts;
};

}

#endif
