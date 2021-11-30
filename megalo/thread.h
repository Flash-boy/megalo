#ifndef MEGALO_THREAD_H_
#define MEGALO_THREAD_H_

#include <pthread.h>
#include <semaphore.h>
#include <memory>
#include <functional>
#include <string>

namespace megalo{

//信号量
class Semaphore{
public:
  Semaphore(uint32_t count = 0);
  ~Semaphore();
  void wait();
  void notify();

private:
  sem_t m_semaphore;
};

// 互斥锁LockGuard
template<class T>
struct ScopedLockImpl{
public:
  ScopedLockImpl(T& mutex)
    :m_mutex(mutex){
      m_mutex.lock();
      m_locked = true;
    }
  ~ScopedLockImpl(){
    unlock();
  }
  void lock(){
    if(!m_locked){
      m_mutex.lock();
      m_locked = true;
    }
  }
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked = false;
    }
  }
private:
  T& m_mutex;
  bool m_locked;
};

// 读锁LockGuard
template<class T>
struct ReadScopedLockImpl{
public:
  ReadScopedLockImpl(T& mutex)
    :m_mutex(mutex){
      m_mutex.rdlock();
      m_locked = true;
    }
  ~ReadScopedLockImpl(){
    unlock();
  }
  void lock(){
    if(!m_locked){
      m_mutex.rdlock();
      m_locked = true;
    }
  }
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked = false;
    }
  }
private:
  T& m_mutex;
  bool m_locked;
};

// 写锁LockGuard
template<class T>
struct WriteScopedLockImpl{
public:
  WriteScopedLockImpl(T& mutex)
    :m_mutex(mutex){
      m_mutex.wrlock();
      m_locked = true;
    }
  ~WriteScopedLockImpl(){
    unlock();
  }
  void lock(){
    if(!m_locked){
      m_mutex.wrlock();
      m_locked = true;
    }
  }
  void unlock(){
    if(m_locked){
      m_mutex.unlock();
      m_locked = false;
    }
  }
private:
  T& m_mutex;
  bool m_locked;
};
// 互斥量
class Mutex{
public:
  typedef ScopedLockImpl<Mutex> Lock;
  Mutex(){
    pthread_mutex_init(&m_mutex, nullptr);
  }
  ~Mutex(){
    pthread_mutex_destroy(&m_mutex);
  }
  void lock(){
    pthread_mutex_lock(&m_mutex);
  }
  void unlock(){
    pthread_mutex_unlock(&m_mutex);
  }
private:
  pthread_mutex_t m_mutex;
};

// 空锁，用于调试
class NullMutex{
public:
  typedef ScopedLockImpl<NullMutex> Lock;
  NullMutex(){
  }
  ~NullMutex(){
  }
  void lock(){
  }
  void unlock(){
  }
private:
};



// 读写互斥量
class RWMutex{
public:
  typedef ReadScopedLockImpl<RWMutex> ReadLock;
  typedef WriteScopedLockImpl<RWMutex> WriteLock;

  RWMutex(){
    pthread_rwlock_init(&m_lock, nullptr);
  }
  ~RWMutex(){
    pthread_rwlock_destroy(&m_lock);
  }
  void rdlock(){
    pthread_rwlock_rdlock(&m_lock);
  }
  void wrlock(){
    pthread_rwlock_wrlock(&m_lock);
  }
  void unlock(){
    pthread_rwlock_unlock(&m_lock);
  }
private:
  pthread_rwlock_t m_lock;

};

// 空读写互斥量,调试用
class NullRWMutex{
public:
  typedef ReadScopedLockImpl<NullRWMutex> ReadLock;
  typedef WriteScopedLockImpl<NullRWMutex> WriteLock;

  NullRWMutex(){
  }
  ~NullRWMutex(){
  }
  void rdlock(){
  }
  void wrlock(){
  }
  void unlock(){
  }
private:
};

//线程封装
class Thread{
public:
  typedef std::shared_ptr<Thread> ptr;
  Thread(std::function<void()> cb, const std::string& name);
  ~Thread();

  pid_t getId() const {return m_id;}
  const std::string& getName() const {return m_name;}

  void join();

  static Thread* GetThis();
  static const std::string& GetName();
  static void SetName(const std::string& name);

private:
  Thread(const Thread&) = delete;
  Thread(const Thread&&) = delete;
  Thread& operator=(const Thread&) = delete;

  static void* run(void* arg);
private:
  pid_t m_id = -1;             //线程id
  pthread_t m_thread = 0;      //线程标识符，句柄
  std::function<void()> m_cb;  //线程函数
  std::string m_name;          //线程名字

  Semaphore m_semaphore;
};

}


#endif