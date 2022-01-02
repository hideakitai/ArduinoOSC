#pragma once
#ifndef ARX_DEBUGLOG_H
#define ARX_DEBUGLOG_H

#ifdef ARDUINO
#include <Arduino.h>
#include <stdarg.h>
#else
#include <iostream>
#include <string>
#include <string.h>
#endif

#include "DebugLog/util/ArxTypeTraits/ArxTypeTraits.h"
#include "DebugLog/util/ArxContainer/ArxContainer.h"
#include "DebugLog/Types.h"
#include "DebugLog/Manager.h"

namespace DebugLog = arx::debug;
using DebugLogLevel = arx::debug::LogLevel;
using DebugLogBase = arx::debug::LogBase;
using DebugLogPrecision = arx::debug::LogPrecision;

// PRINT and PRINTLN are always enabled regardless of log_level
// PRINT and PRINTLN do NOT print to files
#define PRINT(...) DebugLog::Manager::get().print(__VA_ARGS__)
#define PRINTLN(...) DebugLog::Manager::get().println(__VA_ARGS__)
#define LOG_AS_ARR(arr, sz) DebugLog::to_arr(arr, sz)
#define LOG_GET_LEVEL() DebugLog::Manager::get().log_level()
#define LOG_SET_LEVEL(l) DebugLog::Manager::get().log_level(l)
#define LOG_SET_OPTION(file, line, func) DebugLog::Manager::get().option(file, line, func)
#define LOG_SET_DELIMITER(d) DebugLog::Manager::get().delimiter(d)
#define LOG_SET_BASE_RESET(b) DebugLog::Manager::get().base_reset(b)

#ifdef ARDUINO
#define LOG_ATTACH_SERIAL(s) DebugLog::Manager::get().attach(s)
// PRINT_FILE and PRINTLN_FILE are always enabled regardless of file_level
// PRINT_FILE and PRINTLN_FILE do NOT print to Serial
#define PRINT_FILE(...) DebugLog::Manager::get().print_file(__VA_ARGS__)
#define PRINTLN_FILE(...) DebugLog::Manager::get().println_file(__VA_ARGS__)
#define LOG_FILE_FLUSH() DebugLog::Manager::get().flush()
#define LOG_FILE_CLOSE() DebugLog::Manager::get().close()
#define LOG_FILE_IS_OPEN() DebugLog::Manager::get().is_open()
#define LOG_FILE_GET_LEVEL() DebugLog::Manager::get().file_level()
#define LOG_FILE_SET_LEVEL(l) DebugLog::Manager::get().file_level(l)
#define LOG_ATTACH_FS_AUTO(fs, path, mode) DebugLog::Manager::get().attach(fs, path, mode, true)
#define LOG_ATTACH_FS_MANUAL(fs, path, mode) DebugLog::Manager::get().attach(fs, path, mode, false)
#endif  // ARDUINO

#include "DebugLogRestoreState.h"

#endif  // ARX_DEBUGLOG_H
