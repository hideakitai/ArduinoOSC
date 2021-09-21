#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_TRACE
#undef ASSERT
#undef ASSERTM

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                                                                                                    : __FILE__)
#define LOG_ERROR(...) DebugLog::Manager::get().log(arx::debug::LogLevel::ERROR, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...) DebugLog::Manager::get().log(arx::debug::LogLevel::WARN, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...) DebugLog::Manager::get().log(arx::debug::LogLevel::INFO, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...) DebugLog::Manager::get().log(arx::debug::LogLevel::DEBUG, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_TRACE(...) DebugLog::Manager::get().log(arx::debug::LogLevel::TRACE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)

#ifdef ARDUINO
#define ASSERT(b) DebugLog::Manager::get().assertion((b), __FILENAME__, __LINE__, __func__, #b)
#define ASSERTM(b, msg) DebugLog::Manager::get().assertion((b), __FILENAME__, __LINE__, __func__, #b, msg)
#else  // ARDUINO
#include <cassert>
#define ASSERT(b) assert(b)
#define ASSERTM(b, msg) assert((msg, b))
#endif  // ARDUINO
