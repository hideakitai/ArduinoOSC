#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_VERBOSE
#ifdef ARDUINO
#undef LOG_SD_FLUSH
#undef LOG_SD_CLOSE
#endif
#undef ASSERT

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                                                                                                    : __FILE__)
#define LOG_ERROR(...) DebugLog::Manager::get().log(arx::debug::LogLevel::ERRORS, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...) DebugLog::Manager::get().log(arx::debug::LogLevel::WARNINGS, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_VERBOSE(...) DebugLog::Manager::get().log(arx::debug::LogLevel::VERBOSE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)

#ifdef ARDUINO
#define LOG_SD_FLUSH() DebugLog::Manager::get().flush()
#define LOG_SD_CLOSE() DebugLog::Manager::get().close()
#define ASSERT(b) DebugLog::Manager::get().assertion((b), __FILENAME__, __LINE__, __func__, #b)
#else  // ARDUINO
#include <cassert>
#define ASSERT(b) assert(b)
#endif  // ARDUINO
