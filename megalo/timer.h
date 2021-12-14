#ifndef __MEGALO_TIMER_H_
#define __MEGALO_TIMER_H_

#include <memory>
#include <vector>
#include <set>
#include "thread.h"

namespace megalo{

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
  typedef std::shared_ptr<Timer> ptr;

  bool cancel();
  bool refresh();
  bool reset(uint64_t ms, bool from_now);
private:
  Timer(uint64_t ms, std::function<void()> cb, 
        bool recurring, TimerManager* manager);
  Timer(uint64_t next);

private:
  // 是否循环
  bool m_recurring = false;
  // 执行周期
  uint64_t m_ms = 0;
  // 执行时间
  uint64_t m_next = 0;
  std::function<void()> m_cb;
  TimerManager* m_manager = nullptr;

private:
  struct Comparator{
    bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
  };
};


class TimerManager{
friend class Timer;
public:
  typedef RWMutex RWMutexType;

  TimerManager();
  virtual ~TimerManager();
  Timer::ptr addTimer(uint64_t ms, std::function<void()> cb,
                      bool recurring = false);
  Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb,
                              std::weak_ptr<void> weak_cond,
                              bool recurring = false);
  uint64_t getNextTimer();

  void listExpiredCb(std::vector<std::function<void()> >& cbs);
  bool hasTimer();

protected:
  virtual void onTimerInsertAtFront() = 0;
  void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

private:
  bool deleteClockRollover(uint64_t now_ms);
private:
  RWMutexType m_mutex;
  // 定时器集合
  std::set<Timer::ptr, Timer::Comparator> m_timers;
  // 是否出发onTimerInsertAtFront
  bool m_tickled = false;
  // 上次执行的时间
  uint64_t m_previousTime = 0;
};
}

#endif

