#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_TRACE
#undef ASSERT
#undef ASSERTM

#define LOG_SHORT_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG_ERROR(...) DebugLog::Manager::get().log(arx::debug::LogLevel::LVL_ERROR, LOG_SHORT_FILENAME, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...) DebugLog::Manager::get().log(arx::debug::LogLevel::LVL_WARN, LOG_SHORT_FILENAME, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...) DebugLog::Manager::get().log(arx::debug::LogLevel::LVL_INFO, LOG_SHORT_FILENAME, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...) DebugLog::Manager::get().log(arx::debug::LogLevel::LVL_DEBUG, LOG_SHORT_FILENAME, __LINE__, __func__, __VA_ARGS__)
#define LOG_TRACE(...) DebugLog::Manager::get().log(arx::debug::LogLevel::LVL_TRACE, LOG_SHORT_FILENAME, __LINE__, __func__, __VA_ARGS__)

#ifdef ARDUINO
#define ASSERT(b) DebugLog::Manager::get().assertion((b), LOG_SHORT_FILENAME, __LINE__, __func__, #b)
#define ASSERTM(b, msg) DebugLog::Manager::get().assertion((b), LOG_SHORT_FILENAME, __LINE__, __func__, #b, msg)
#else  // ARDUINO
#include <cassert>
#define ASSERT(b) assert(b)
#define ASSERTM(b, msg) assert((msg, b))
#endif  // ARDUINO
