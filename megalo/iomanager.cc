#include "iomanager.h"
#include "macro.h"
#include "log.h"
#include <functional>
#include <stdexcept>
#include <sys/epoll.h>
#include <fcntl.h>

namespace megalo{

static megalo::Logger::ptr g_logger = MEGALO_LOG_NAME("system");

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event){
  switch(event){
    case IOManager::READ:
      return read;
    case IOManager::WRITE:
      return write;
    default:
      MEGALO_ASSERT2(false, "getContext");
  }
  throw std::invalid_argument("getContext invalid event");
}

void IOManager::FdContext::resetContext(EventContext& ctx){
  ctx.scheduler = nullptr;
  ctx.fiber.reset();
  ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event){
  MEGALO_ASSERT(events & event);
  events = (Event)(events & ~event);
  EventContext& ctx = getContext(event);
  if(ctx.cb){
    ctx.scheduler->schedule(&ctx.cb);
  }else{
    ctx.scheduler->schedule(&ctx.fiber);
  }
  ctx.scheduler = nullptr;
}


IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
  :Scheduler(threads, use_caller, name){
    m_epfd = epoll_create(5000);
    MEGALO_ASSERT(m_epfd > 0);

    int rt = pipe(m_tickleFds);
    MEGALO_ASSERT(!rt);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    MEGALO_ASSERT(!rt);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    MEGALO_ASSERT(!rt);

    contextResize(32);
    start();
}

void IOManager::contextResize(size_t size){
  m_fdContexts.resize(size);
  for(size_t i = 0; i < m_fdContexts.size(); ++i){
    if(!m_fdContexts[i]){
      m_fdContexts[i] = new FdContext;
      m_fdContexts[i]->fd = i;
    }
  }
}

IOManager::~IOManager(){
  stop();
  close(m_epfd);
  close(m_tickleFds[0]);
  close(m_tickleFds[1]);

  for(size_t i = 0; i < m_fdContexts.size(); ++i){
    if(m_fdContexts[i]){
      delete m_fdContexts[i];
    }
  }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb){
  FdContext* fd_ctx = nullptr;
  RWMutexType::ReadLock lock(m_mutex);
  if((int)m_fdContexts.size() > fd){
    fd_ctx = m_fdContexts[fd];
    lock.unlock();
  }else{
    lock.unlock();
    RWMutexType::WriteLock lock2(m_mutex);
    contextResize(fd * 1.5);
    fd_ctx = m_fdContexts[fd];
  }

  FdContext::MutexType::Lock lock2(fd_ctx->mutex);
  if(fd_ctx->events & event){
    MEGALO_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd
                  << " event=" << event 
                  << " fd_ctx.event=" << fd_ctx->events;
    MEGALO_ASSERT(!(event & fd_ctx->events));
  }

  int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
  epoll_event epevent;
  epevent.events = EPOLLET | fd_ctx->events | event;
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if(rt){
    MEGALO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
          << op << ", " << fd;
    return -1;
  }
  ++m_pendingEventCount;
  fd_ctx->events = (Event)(fd_ctx->events | event);
  FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
  MEGALO_ASSERT(!event_ctx.scheduler
                && !event_ctx.cb
                && !event_ctx.fiber);

  event_ctx.scheduler = Scheduler::GetThis();
  if(cb){
    event_ctx.cb.swap(cb);
  }else{
    event_ctx.fiber = Fiber::GetThis();
    MEGALO_ASSERT2(event_ctx.fiber->getState() == Fiber::EXEC
            , "state=" << event_ctx.fiber->getState());
  }
  return 0;
}
bool IOManager::delEvent(int fd, Event event){
  RWMutexType::ReadLock lock(m_mutex);
  if((int)m_fdContexts.size() <= fd){
    return false;
  }
  FdContext* fd_ctx = m_fdContexts[fd];
  lock.unlock();

  FdContext::MutexType::Lock lock2(fd_ctx->mutex);
  if(!(fd_ctx->events & event)){
    return false;
  }

  Event new_event = (Event)(fd_ctx->events & ~event);
  int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
  
  epoll_event epevent;
  epevent.events = EPOLLET | new_event;
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if(rt){
    MEGALO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
          << op << ", " << fd;
    return false;
  }

  --m_pendingEventCount;
  fd_ctx->events = new_event;
  FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
  fd_ctx->resetContext(event_ctx);
  return true;
}

bool IOManager::cancelEvent(int fd, Event event){
  RWMutexType::ReadLock lock(m_mutex);
  if((int)m_fdContexts.size() <= fd){
    return false;
  }
  FdContext* fd_ctx = m_fdContexts[fd];
  lock.unlock();

  FdContext::MutexType::Lock lock2(fd_ctx->mutex);
  if(!(fd_ctx->events & event)){
    return false;
  }

  Event new_event = (Event)(fd_ctx->events & ~event);
  int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
  
  epoll_event epevent;
  epevent.events = EPOLLET | new_event;
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if(rt){
    MEGALO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
          << op << ", " << fd;
    return false;
  }
  
  fd_ctx->triggerEvent(event);
  --m_pendingEventCount;
  return true;
}
bool IOManager::cancelAll(int fd){
  RWMutexType::ReadLock lock(m_mutex);
  if((int)m_fdContexts.size() <= fd){
    return false;
  }
  FdContext* fd_ctx = m_fdContexts[fd];
  lock.unlock();

  FdContext::MutexType::Lock lock2(fd_ctx->mutex);
  if(!fd_ctx->events){
    return false;
  }

  int op = EPOLL_CTL_DEL;
  epoll_event epevent;
  epevent.events = 0;
  epevent.data.ptr = fd_ctx;

  int rt = epoll_ctl(m_epfd, op, fd, &epevent);
  if(rt){
    MEGALO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
          << op << ", " << fd;
    return false;
  }

  if(fd_ctx->events & READ){
    fd_ctx->triggerEvent(READ);
    --m_pendingEventCount;
  }
  if(fd_ctx->events & WRITE){
    fd_ctx->triggerEvent(WRITE);
    --m_pendingEventCount;
  }
  MEGALO_ASSERT(fd_ctx->events == 0);
  return true;
}

IOManager* IOManager::GetThis(){
  return dynamic_cast<IOManager*>(Scheduler::GetThis());
}


void IOManager::tickle(){
  if(!hasIdleThread()){
    return;
  } 
  
  int rt = write(m_tickleFds[1], "T", 1);
  MEGALO_ASSERT(rt == 1);
} 

bool IOManager::stopping(){
  uint64_t timeout = 0;
  return stopping(timeout);
} 

bool IOManager::stopping(uint64_t& timeout){
  timeout = getNextTimer();
  return timeout == ~0ull
    && m_pendingEventCount == 0
    && Scheduler::stopping();
}
void IOManager::idle(){
  MEGALO_LOG_DEBUG(g_logger) << "idle";
  const uint64_t MAX_EVENTS = 256;
  epoll_event* events = new epoll_event[MAX_EVENTS]();
  std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
      delete[] ptr;
  });

  while(true){
    uint64_t next_timeout = 0;
    if(stopping(next_timeout)){
      MEGALO_LOG_INFO(g_logger) << "name=" << getName() 
                                << " idle stopping idle";
      break;
    }

    int rt = 0;
    do{
      static const int MAX_TIMEOUT = 1000;
      if(next_timeout != ~0ull){
        next_timeout = (int)next_timeout > MAX_TIMEOUT
                        ? MAX_TIMEOUT : next_timeout;
      }else{
        next_timeout = MAX_TIMEOUT;
      }
      rt = epoll_wait(m_epfd, events, MAX_EVENTS, next_timeout);
      if(rt < 0 && errno == EINTR){
      }else{
        break;
      }
    }while(true);

    std::vector<std::function<void()> > cbs;
    listExpiredCb(cbs);
    if(!cbs.empty()){
      schedule(cbs.begin(), cbs.end());
      cbs.clear();
    }

    for(int i = 0; i < rt; ++i){
      epoll_event& event = events[i];
      if(event.data.fd == m_tickleFds[0]){
        uint8_t dummy[256];
        while(read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
        continue;
      }

      FdContext* fd_ctx = (FdContext*)event.data.ptr;
      FdContext::MutexType::Lock lock(fd_ctx->mutex);
      if(event.events & (EPOLLERR | EPOLLHUP)){
        event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
      }
      int real_events = NONE;
      if(event.events & EPOLLIN){
        real_events |= READ;
      }
      if(event.events & EPOLLOUT){
        real_events |= WRITE;
      }

      if((fd_ctx->events & real_events) == NONE){
        continue;
      }

      int left_events = (fd_ctx->events & ~real_events);
      int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
      event.events = EPOLLET | left_events;

      int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
      if(rt2){
        MEGALO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
              << op << ", " << fd_ctx->fd;
        continue;
      }

      if(real_events & READ){
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
      }
      if(real_events & WRITE){
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
      }
    }

    Fiber::ptr cur = Fiber::GetThis();
    auto raw_ptr = cur.get();
    cur.reset();

    raw_ptr->swapOut();
  }
}

void IOManager::onTimerInsertAtFront(){
  tickle();
}

}
