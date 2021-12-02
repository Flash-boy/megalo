#ifndef MEGALO_FIBER_H_
#define MEGALO_FIBER_H_

#include <memory>
#include <ucontext.h>
#include <functional>
#include "thread.h"

namespace megalo{

class Fiber: public std::enable_shared_from_this<Fiber>{
public:
  typedef std::shared_ptr<Fiber> ptr;

  enum State{
    INIT,  //初始化
    HOLD,  //
    EXEC,  //执行中
    TERM,  //执行结束
    READY,
    EXCEPT
  };
private:
  Fiber();
public:
  Fiber(std::function<void()> cb, size_t stack_size = 0);
  ~Fiber();
  // 重置协程函数，并重置状态
  // INIT, TERM
  void reset(std::function<void()> cb);
  // 切换到当前协程
  void swapIn();
  // 切换到后台执行
  void swapOut();

  uint64_t getId() const {return m_id;}
public:
  // 返回当前执行点协程
  static Fiber::ptr GetThis();
  // 设置当前协程序
  static void SetThis(Fiber* f);
  // 协程切换到后台,并设置为READY状态
  static void YieldToReady();
  // 协程切换到后台,并设置为HOLD状态
  static void YieldToHold();
  // 总协程数
  static uint64_t TotalFibers();
  // 协程函数
  static void MainFunc();
  // 获取FiberId
  static uint64_t GetFiberId();
private:

  uint64_t m_id = 0;             // 协程id
  uint32_t m_stacksize = 0;      // 栈大小
  State m_state = INIT;          // 协程状态
  ucontext_t m_ctx;              // 协程上下文
  void* m_stack = nullptr;       // 栈指针
  std::function<void()> m_cb;    // 协程实际函数

};

}

#endif
