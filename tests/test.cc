#include <iostream>
#include "../megalo/log.h"
#include "../megalo/util.h"
int main(){
  megalo::Logger::ptr logger(new megalo::Logger);
  logger->addLogAppender(megalo::LogAppender::ptr(new megalo::StdoutLogAppender));

  megalo::FileLogAppender::ptr file_appender(new megalo::FileLogAppender("./log.txt"));
  megalo::LogFormatter::ptr fmt(new megalo::LogFormatter("%d%T%p%T%m%n"));
  file_appender->setFormatter(fmt);
  file_appender->setLevel(megalo::LogLevel::ERROR);

  logger->addLogAppender(file_appender);

  MEGALO_LOG_DEBUG(logger) << "hello megalo log";
  MEGALO_LOG_ERROR(logger) << "test macro error";

  MEGALO_LOG_FMT_DEBUG(logger, "test fmt macro %s","xxx");

  auto l = megalo::LoggerMgr::GetInstance()->getLogger("xx");
  MEGALO_LOG_ERROR(l) << "XXX";

  return 0;
}
