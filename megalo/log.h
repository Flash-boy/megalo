#ifndef _MEGALO_LOG_H_
#define _MEGALO_LOG_H_

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdarg.h>
#include <map>

#include "util.h"
#include "singleton.h"
#include "thread.h"

//使用流式方式把level日志写入到logger
#define MEGALO_LOG_LEVEL(logger,level) \
  if(logger->getLevel() <= level) \
    megalo::LogEventWrap(megalo::LogEvent::ptr(new megalo::LogEvent(logger,level, \
            __FILE__,__LINE__,0,megalo::GetThreadId(),megalo::GetFiberId(), \
            time(0),megalo::Thread::GetName()))).getSS()
  
#define MEGALO_LOG_DEBUG(logger) MEGALO_LOG_LEVEL(logger,megalo::LogLevel::DEBUG)
#define MEGALO_LOG_INFO(logger) MEGALO_LOG_LEVEL(logger,megalo::LogLevel::INFO)
#define MEGALO_LOG_WARN(logger) MEGALO_LOG_LEVEL(logger,megalo::LogLevel::WARN)
#define MEGALO_LOG_ERROR(logger) MEGALO_LOG_LEVEL(logger,megalo::LogLevel::ERROR)
#define MEGALO_LOG_FATAL(logger) MEGALO_LOG_LEVEL(logger,megalo::LogLevel::FATAL)

//使用格式化方式将level日志写入到logger
#define MEGALO_LOG_FMT_LEVEL(logger,level,fmt,...) \
  if(logger->getLevel() <= level) \
    megalo::LogEventWrap(megalo::LogEvent::ptr(new megalo::LogEvent(logger,level, \
            __FILE__,__LINE__,0,megalo::GetThreadId(),megalo::GetFiberId() \
          ,time(0),megalo::Thread::GetName()))).getEvent()->format(fmt,__VA_ARGS__)

#define MEGALO_LOG_FMT_DEBUG(logger,fmt,...) MEGALO_LOG_FMT_LEVEL(logger,megalo::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define MEGALO_LOG_FMT_INFO(logger,fmt,...) MEGALO_LOG_FMT_LEVEL(logger,megalo::LogLevel::INFO,fmt,__VA_ARGS__)
#define MEGALO_LOG_FMT_WARN(logger,fmt,...) MEGALO_LOG_FMT_LEVEL(logger,megalo::LogLevel::WARN,fmt,__VA_ARGS__)
#define MEGALO_LOG_FMT_ERROR(logger,fmt,...) MEGALO_LOG_FMT_LEVEL(logger,megalo::LogLevel::ERROR,fmt,__VA_ARGS__)
#define MEGALO_LOG_FMT_FATAL(logger,fmt,...) MEGALO_LOG_FMT_LEVEL(logger,megalo::LogLevel::FATAL,fmt,__VA_ARGS__)


// 获取主日志器
#define MEGALO_LOG_ROOT() megalo::LoggerMgr::GetInstance()->getRoot()
// 获取name名字的日志器
#define MEGALO_LOG_NAME(name) megalo::LoggerMgr::GetInstance()->getLogger(name)


namespace megalo{

class Logger;

//日志级别
class LogLevel{
public:
  enum Level{
    UNKNOWM = 0,
    DEBUG   = 1,
    INFO    = 2,
    WARN    = 3,
    ERROR   = 4,
    FATAL   = 5
  };
  static const char* ToString(LogLevel::Level level);
  static LogLevel::Level FromString(const std::string& str);
};

//日志事件
class LogEvent{
public:
  typedef std::shared_ptr<LogEvent> ptr;
  LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file, 
      int32_t line,uint32_t elapse,uint32_t thread_id, 
      uint32_t fiber_id,uint64_t time,const std::string& thread_name);

  const char* getFile()const {return m_file;}
  int32_t getLine()const {return m_line;}
  uint32_t getElapse()const {return m_elapse;}
  uint32_t getThreadId()const {return m_threadId;}
  uint32_t getFiberId()const {return m_fiberId;}
  uint64_t getTime()const {return m_time;} 
  std::shared_ptr<Logger> getLogger()const {return m_logger;}
  LogLevel::Level getLevel()const {return m_level;}
  std::string getThreadName()const {return m_threadName;}

  std::string getContent(){return m_ss.str();}
  std::stringstream& getSS(){return m_ss;}

  void format(const char* fmt,...);
  void format(const char* fmt,va_list al);
  

private:
  const char* m_file = nullptr;   //文件名
  int32_t m_line = 0;             //行号
  uint32_t m_elapse = 0;          //程序启动开始到现在的时间
  uint32_t m_threadId = 0;        //线程id
  uint32_t m_fiberId = 0;         //协程id
  uint64_t m_time;                //时间戳
  std::string m_threadName;       //线程名称
  std::stringstream m_ss;         //日志内容流
  std::shared_ptr<Logger> m_logger;
  LogLevel::Level m_level;

};

// 日志事件包装器
class LogEventWrap{
public:
  LogEventWrap(LogEvent::ptr e);
  ~LogEventWrap();
  LogEvent::ptr getEvent()const {return m_event;}
  std::stringstream& getSS();
private:
  LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter{
public:
  typedef std::shared_ptr<LogFormatter> ptr;
  class FormatItem{
  public:
    FormatItem(const std::string& str=""){}
    typedef std::shared_ptr<FormatItem> ptr;
    virtual ~FormatItem(){}
    virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
  };

  LogFormatter(const std::string& pattern);
  std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
  bool isError() const {return m_error;}
  const std::string getPattern() const {return m_pattern;}
private:
  void init();
private:
  std::string m_pattern;
  std::vector<FormatItem::ptr> m_items;
  bool m_error = false;
};

// 日志输出地
class LogAppender{
friend class Logger;
public:
  typedef std::shared_ptr<LogAppender> ptr;
  typedef Spinlock MutexType;

  virtual ~LogAppender(){}
  virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
  void setFormatter(LogFormatter::ptr val){ m_formatter = val;}
  void setLevel(LogLevel::Level level){m_level = level;}
  LogFormatter::ptr getFormatter(){ return m_formatter;}
  virtual std::string toYamlString() = 0;
protected:
  LogLevel::Level m_level = LogLevel::DEBUG;
  LogFormatter::ptr m_formatter;
  bool m_hasFormatter = false;
  MutexType m_mutex;
};


// 日志输出器
class Logger: public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
  typedef Spinlock MutexType;
  typedef std::shared_ptr<Logger> ptr;
  Logger(const std::string name = "root");
  void log(LogLevel::Level level,LogEvent::ptr event);
  void debug(LogEvent::ptr event);
  void info(LogEvent::ptr event);
  void warn(LogEvent::ptr event);
  void error(LogEvent::ptr event);
  void fatal(LogEvent::ptr event);

  void addLogAppender(LogAppender::ptr appender);
  void delLogAppender(LogAppender::ptr appender);
  std::string getName()const {return m_name;}
  LogLevel::Level getLevel()const {return m_level;}
  void setLevel(LogLevel::Level val) {m_level = val;}
  void clearAppenders() {m_appenders.clear();}
  void setFormatter(LogFormatter::ptr val);
  void setFormatter(const std::string& val);
  LogFormatter::ptr getFormatter();
  std::string toYamlString();

private:
  std::string m_name;                      //日志名称
  LogLevel::Level m_level;                 //日志级别
  std::list<LogAppender::ptr> m_appenders; //appender集合
  LogFormatter::ptr m_formatter;           //日志格式器
  Logger::ptr m_root;                      //主日志器
  MutexType m_mutex; 
};

// 输出到控制台appender
class StdoutLogAppender : public LogAppender{
public:
  typedef std::shared_ptr<StdoutLogAppender> ptr;
  void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
  std::string toYamlString() override;
private:

};

// 输出到文件appender
class FileLogAppender: public LogAppender{
public:
  typedef std::shared_ptr<FileLogAppender> ptr;
  FileLogAppender(const std::string& filename);
  void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
  std::string toYamlString() override;
  bool reopen();
private:
  std::string m_filename;
  std::ofstream m_filestream;    
  uint64_t m_lastTime = 0;
};

class LoggerManager{
public:
  typedef Spinlock MutexType;
  LoggerManager();
  Logger::ptr getLogger(const std::string& name);
  void init();
  Logger::ptr getRoot()const {return m_root;}
  std::string toYamlString();

private:
  std::map<std::string,Logger::ptr> m_loggers;  //日志器容器
  Logger::ptr m_root;                           //主日志器
  MutexType m_mutex;
};

//日志管理器单例模式
typedef megalo::Singleton<LoggerManager> LoggerMgr;

}
#endif
