#ifndef __MEGALO_SCHEDULER_H_
#define __MEGALO_SCHEDULER_H_

#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include "thread.h"
#include "fiber.h"

namespace megalo{

class Scheduler{
public: typedef std::shared_ptr<Scheduler> ptr; typedef Mutex MutexType; 
  Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
  virtual ~Scheduler();

  const std::string& getName() const {return m_name;}
  static Scheduler* GetThis();
  static Fiber* GetMainFiber();

  void start();
  void stop();

public:
  template<class FiberOrCb>
  void scheduler(FiberOrCb fc, int thread = -1){
    bool need_tickle = false;
    {
      MutexType lock(m_mutex);
      need_tickle = schedulerNoLock(fc, thread);
    }

    if(need_tickle){
      tickle();
    }
  }
  template<class InputIterator>
  void scheduler(InputIterator begin, InputIterator end, int thread = -1){
    bool need_tickle = false;
    {
      MutexType lock(m_mutex);
      while(begin != end){
        need_tickle = schedulerNoLock(&*begin, thread) || need_tickle;
        ++begin;
      }
    }
    if(need_tickle){
      tickle();
    }
  }

protected:
virtual void tickle();
void run();
virtual bool stopping();
virtual void idle();
void setThis();

private:
  template<class FiberOrCb>
  bool schedulerNoLock(FiberOrCb fc, int thread){
    bool need_tickle = m_fibers.empty();
    FiberAndThread ft(fc, thread);
    if(ft.fiber || ft.cb){
      m_fibers.push_back(ft);
    }
    return need_tickle;
  }
private:
  struct FiberAndThread{
    Fiber::ptr fiber;
    std::function<void()> cb;
    int thread;
    FiberAndThread(Fiber::ptr f,int thr)
        :fiber(f)
        ,thread(thr){
    }
    FiberAndThread(Fiber::ptr* f,int thr)
        :thread(thr){
        fiber.swap(*f);
    }
    FiberAndThread(std::function<void()> f, int thr)
        :cb(f)
        ,thread(thr){
    }
    FiberAndThread(std::function<void()>* f, int thr)
        :thread(thr){
        cb.swap(*f);
    }
    FiberAndThread()
        :thread(-1){
    }
    void reset(){
      fiber = nullptr;
      cb = nullptr;
      thread = -1;
    }
  };
private:
  MutexType m_mutex;
  std::vector<Thread::ptr> m_threads;
  std::list<FiberAndThread> m_fibers;
  std::string m_name;
  Fiber::ptr m_rootFiber;

protected:
  std::vector<int> m_threadIds;
  size_t m_threadCount = 0;
  std::atomic<size_t> m_activeThreadCount = {0};
  std::atomic<size_t> m_idleThreadCount = {0};
  bool m_stopping = true;
  bool m_autoStop = false;
  int m_rootThread = 0;

};
}

#endif
