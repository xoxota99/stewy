#pragma once
#ifndef __STU_LOGGER_H__
#define __STU_LOGGER_H__
/*
6dof-stewduino
Copyright (C) 2018  Philippe Desrosiers

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
// #include <math.h>
#include "Arduino.h"

class Logger {
public:
  enum LogLevel{
    TRACE=0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
  };

  char* lvlStrings[6] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
  };

  LogLevel level = TRACE;
  static Logger* instance();

  void log(const LogLevel level, const char* format, ...);
  // void log(const LogLevel level, const char* msg);

  void trace(const char * msg);
  void debug(const char * msg);
  void info(const char * msg);
  void warn(const char * msg);
  void error(const char * msg);
  void fatal(const char * msg);

  void trace(const char * format, ...);
  void debug(const char * format, ...);
  void info(const char * format, ...);
  void warn(const char * format, ...);
  void error(const char * format, ...);
  void fatal(const char * format, ...);

private:
  void _log_va_list(const LogLevel level, const char* format, va_list args);

  static Logger* m_pInstance;
  Logger(){};  // Private so that it can  not be called
  Logger(Logger const&){};             // copy constructor is private
  Logger& operator=(Logger const&){};  // assignment operator is private

};

#endif      //__STU_LOGGER_H__
