#include "Logger.h"
// Singleton instance
Logger* Logger::m_pInstance = NULL;
Logger* Logger::instance()
{
  if (!m_pInstance)  // Only allow one instance of class to be generated.
  m_pInstance = new Logger;

  return m_pInstance;
}

void Logger::_log_va_list(const LogLevel level, const char* fmt, va_list args){
  char buffer[256];
  // va_start (args, fmt);
  vsprintf (buffer,fmt, args);
  Serial.printf("[%s] - %s",lvlStrings[level],buffer);
  // va_end (args);
}

void Logger::log(const LogLevel level, const char* fmt, ...){
  char buffer[256];
  va_list args;
  va_start(args,fmt);
  _log_va_list(level,fmt,args);
  va_end(args);
}

// void Logger::log(const LogLevel level, const char* msg){
//   Serial.printf("[%s] - %s",lvlStrings[level],msg);
// }

void Logger::trace(const char * msg){
  log(TRACE,msg);
}

void Logger::debug(const char * msg){
  log(DEBUG,msg);
}

void Logger::info(const char * msg){
  log(INFO,msg);
}

void Logger::warn(const char * msg){
  log(WARN,msg);
}

void Logger::error(const char * msg){
  log(ERROR,msg);
}

void Logger::fatal(const char * msg){
  log(FATAL,msg);
}

void Logger::trace(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(TRACE,fmt, args);
  va_end(args);
}

void Logger::debug(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(DEBUG,fmt, args);
  va_end(args);
}

void Logger::info(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(INFO,fmt, args);
  va_end(args);
}

void Logger::warn(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(WARN,fmt, args);
  va_end(args);
}

void Logger::error(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(ERROR,fmt, args);
  va_end(args);
}

void Logger::fatal(const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  _log_va_list(FATAL,fmt, args);
  va_end(args);
}
