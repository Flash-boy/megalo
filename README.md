# megalo服务器框架开发

## 1.日志模块

支持流式日志风格和格式化日志风格写日志，支持自定义日志格式：

| 格式       |含义    |
| -----| ----|
| %m |消息 |
|   %p     | 日志等级   |
|   %r     | 累积毫秒数 |
|   %c     | 日志名称   |
|   %t     | 线程id   |
|   %d     | 时间   |
| %n | 换行 |
|   %f     | 文件名   |
|   %l     | 行号   |
|   %T     | Tab   |
|   %F     | 协程id   |
|   %N     | 线程名称   |

使用方式

```cpp
megalo::Logger::pte g_logger = MEGALO_LOG_ROOT();
MEGALO_LOG_INFO(g_logger) << "this is a log";
MEGALO_LOG_FMT_INFO(g_logger) << "this is a log";
```



## 2.配置模块

采用YAML文件作为配置内容，定义YAML内容既可以直接使用。支持变更配置通知功能。支持常见的数据类型，包括STL常见容器（vector, list, set, map)以及自定义类型（需要实现序列化和反序列化方法）。

常见配置如下

```yaml
logs:
  - name: root
    level: info
    formatter: "%d%T%m%n"
    appenders: 
      - type: FileLogAppender
        file: log.txt
      - type: StdoutLogAppender
  - name: system
    level: debug
    formatter: "%d%T%m%n"
    appenders: 
      - type: FileLogAppender
        file: log.txt
      - type: StdoutLogAppender
```

## 3.线程模块

封装了pthread线程函数，以及一些常用的并发编程对象，包括Thread, Semaphore, Mutex, RWMutex, Spinlock等。扩展了Thread对象，包括线程名，线程id等。

## 4.协程模块

基于ucontext实现了用户态协程

## 5.协程调度模块

协程调度器，管理协程的调度，内部实现为一个线程池，支持协程在多线程中切换，也可以指定协程在固定的线程中执行。是一个N-M的协程调度模型，N个线程，M个协程。重复利用每一个线程。

