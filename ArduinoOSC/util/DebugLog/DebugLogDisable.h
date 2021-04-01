#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_VERBOSE
#ifdef ARDUINO
#undef LOG_SD_FLUSH
#undef LOG_SD_CLOSE
#endif
#undef ASSERT

#define LOG_ERROR(...) ((void)0)
#define LOG_WARNING(...) ((void)0)
#define LOG_VERBOSE(...) ((void)0)
#ifdef ARDUINO
#define LOG_SD_FLUSH() ((void)0)
#define LOG_SD_CLOSE() ((void)0)
#endif
#define ASSERT(b) ((void)0)
