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


namespace arx {
namespace debug {

    namespace detail
    {
        template <class T> struct remove_reference      { using type = T; };
        template <class T> struct remove_reference<T&>  { using type = T; };
        template <class T> struct remove_reference<T&&> { using type = T; };
        template <class T> constexpr T&& forward(typename remove_reference<T>::type& t) noexcept { return static_cast<T&&>(t); }
        template <class T> constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept { return static_cast<T&&>(t); }
    }

// serial loggers
#ifdef ARDUINO
    using string_t = String;
#else
    using string_t = std::string;
#endif

// sd loggers
#ifdef ARDUINO

    struct Logger
    {
        virtual ~Logger() {}
        virtual void flush() = 0;

        virtual size_t print(const __FlashStringHelper*) = 0;
        virtual size_t print(const String&) = 0;
        virtual size_t print(const char[]) = 0;
        virtual size_t print(char) = 0;
        virtual size_t print(unsigned char, int = DEC) = 0;
        virtual size_t print(int, int = DEC) = 0;
        virtual size_t print(unsigned int, int = DEC) = 0;
        virtual size_t print(long, int = DEC) = 0;
        virtual size_t print(unsigned long, int = DEC) = 0;
        virtual size_t print(double, int = 2) = 0;
        virtual size_t print(const Printable&) = 0;

        virtual size_t println(const __FlashStringHelper*) = 0;
        virtual size_t println(const String&) = 0;
        virtual size_t println(const char[]) = 0;
        virtual size_t println(char) = 0;
        virtual size_t println(unsigned char, int = DEC) = 0;
        virtual size_t println(int, int = DEC) = 0;
        virtual size_t println(unsigned int, int = DEC) = 0;
        virtual size_t println(long, int = DEC) = 0;
        virtual size_t println(unsigned long, int = DEC) = 0;
        virtual size_t println(double, int = 2) = 0;
        virtual size_t println(const Printable&) = 0;
        virtual size_t println(void) = 0;
    };

    template <typename SdType, typename FileType>
    class SdLogger : public Logger
    {
        SdType* sd;
        String path;
        FileType file;

    public:

        SdLogger(SdType& sdclass, const String& path)
        : sd(&sdclass)
        , path(path)
        {
#ifdef O_WRITE
            file = sd->open(path.c_str(), O_WRITE | O_CREAT);
#elif defined (FILE_WRITE)
            file = sd->open(path.c_str(), FILE_WRITE);
#endif
        }

        virtual ~SdLogger() { if (file) file.close(); }
        virtual void flush() override { file.flush(); }

        virtual size_t print(const __FlashStringHelper* x) override { return file.print(x); }
        virtual size_t print(const String& x) override { return file.print(x); }
        virtual size_t print(const char x[]) override { return file.print(x); }
        virtual size_t print(char x) override { return file.print(x); }
        virtual size_t print(unsigned char x, int b = DEC) override { return file.print(x, b); }
        virtual size_t print(int x, int b = DEC) override { return file.print(x, b); }
        virtual size_t print(unsigned int x, int b = DEC) override { return file.print(x, b); }
        virtual size_t print(long x, int b = DEC) override { return file.print(x, b); }
        virtual size_t print(unsigned long x, int b = DEC) override { return file.print(x, b); }
        virtual size_t print(double x, int b = 2) override { return file.print(x, b); }
        virtual size_t print(const Printable& x) override { return file.print(x); }

        virtual size_t println(const __FlashStringHelper* x) override { return file.println(x); }
        virtual size_t println(const String& x) override { return file.println(x); }
        virtual size_t println(const char x[]) override { return file.println(x); }
        virtual size_t println(char x) override { return file.println(x); }
        virtual size_t println(unsigned char x, int b = DEC) override { return file.println(x, b); }
        virtual size_t println(int x, int b = DEC) override { return file.println(x, b); }
        virtual size_t println(unsigned int x, int b = DEC) override { return file.println(x, b); }
        virtual size_t println(long x, int b = DEC) override { return file.println(x, b); }
        virtual size_t println(unsigned long x, int b = DEC) override { return file.println(x, b); }
        virtual size_t println(double x, int b = 2) override { return file.println(x, b); }
        virtual size_t println(const Printable& x) override { return file.println(x); }
        virtual size_t println(void) override { return file.println(); }
    };

#endif // sd loggers

    enum class LogLevel {NONE, ERRORS, WARNINGS, VERBOSE};

    class Manager
    {
#ifdef ARDUINO
        Stream* stream {&Serial};
        Logger* logger {nullptr};
#endif
        LogLevel log_level = LogLevel::VERBOSE;
        string_t delim {" "};
        bool b_file {true};
        bool b_line {true};
        bool b_func {true};
        bool b_auto_save {false};
        bool b_only_sd {false};

        // singleton
        Manager() {}
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;

    public:

        ~Manager()
        {
#ifdef ARDUINO
            if (logger) delete logger;
#endif
        }

        static Manager &get()
        {
            static Manager m;
            return m;
        }

#ifdef ARDUINO
        void attach(Stream& s) { stream = &s; }

        void flush()
        {
            if (logger) logger->flush();
        }

        void close()
        {
            flush();
            if (logger) delete logger;
        }

#ifdef FILE_WRITE
        template <typename SdType>
        void attach(SdType& s, const String& path, bool auto_save, bool only_sd = false)
        {
            close();
            logger = new SdLogger<SdType, File>(s, path);
            b_auto_save = auto_save;
            b_only_sd = only_sd;
        }
#endif

#endif // ARDUINO

        void print()
        {
#ifdef ARDUINO
            if (logger && b_auto_save) logger->flush();
#endif
        }

        template<typename Head, typename... Tail>
        void print(Head&& head, Tail&&... tail)
        {
#ifdef ARDUINO
            if (!b_only_sd)
            {
                stream->print(head);
                if (sizeof...(tail) != 0)
                    stream->print(delim);
            }
            if (logger)
            {
                logger->print(head);
                if (sizeof...(tail) != 0)
                    logger->print(delim);
            }
            print(detail::forward<Tail>(tail)...);
#else
            std::cout << head;
            if (sizeof...(tail) != 0)
                std::cout << delim;
            print(std::forward<Tail>(tail)...);
#endif
        }

        void println()
        {
#ifdef ARDUINO
            if (!b_only_sd)
            {
                stream->println();
            }
            if (logger)
            {
                logger->println();
                if (b_auto_save) logger->flush();
            }
#else
            std::cout << std::endl;
#endif
        }

        template<typename Head, typename... Tail>
        void println(Head&& head, Tail&&... tail)
        {
#ifdef ARDUINO
            if (!b_only_sd)
            {
                stream->print(head);
                if (sizeof...(tail) != 0)
                    stream->print(delim);
            }
            if (logger)
            {
                logger->print(head);
                if (sizeof...(tail) != 0)
                    logger->print(delim);
            }
            println(detail::forward<Tail>(tail)...);
#else
            std::cout << head;
            if (sizeof...(tail) != 0)
                std::cout << delim;
            println(std::forward<Tail>(tail)...);
#endif
        }

#ifdef ARDUINO
        void assertion(bool b, const char* file, int line, const char* func, const char* expr)
        {
            while (!b)
            {
                string_t str = string_t("[ASSERT] ") + file + string_t(" ") + line + string_t(" ") + func + string_t(" : ") + expr;
                println(str);
                if (logger) logger->flush();
                delay(1000);
            }
        }
#endif

        void logLevel(const LogLevel l) { log_level = l; }
        LogLevel logLevel() { return log_level; }

        template <typename... Args>
        void log(LogLevel level, const char* file, int line, const char* func, Args&&... args)
        {
            if ((log_level == LogLevel::NONE) || (level == LogLevel::NONE)) return;
            if ((int)level <= (int)log_level)
            {
                string_t lvl_str;
                switch (level)
                {
                    case LogLevel::ERRORS:   lvl_str = "[ERROR] ";   break;
                    case LogLevel::WARNINGS: lvl_str = "[WARNING] "; break;
                    case LogLevel::VERBOSE:  lvl_str = "[VERBOSE] "; break;
                    default:                 lvl_str = "";           break;
                }

                string_t header = lvl_str;
#ifdef ARDUINO
                if (b_file) header += file + string_t(" ");
                if (b_line) header += string_t("L.") + line + string_t(" ");
                if (b_func) header += func + string_t(" ");
                header += string_t(": ");
                print(header);
                println(detail::forward<Args>(args)...);
#else
                if (b_file) { header += file; header += " "; };
                if (b_line) { header += "L."; header += std::to_string(line); header += " "; }
                if (b_func) { header += func; header += " "; } ;
                header += ": ";
                print(header);
                println(std::forward<Args>(args)...);
#endif
            }
        }

        void option(const bool en_file, const bool en_line, const bool en_func)
        {
            b_file = en_file;
            b_line = en_line;
            b_func = en_func;
        }

        void delimiter(const string_t& del)
        {
            delim = del;
        }
    };

} // namespace debug
} // namespace ard


namespace DebugLog = arx::debug;
using DebugLogLevel = arx::debug::LogLevel;

#ifdef NDEBUG

#define PRINT(...) ((void)0)
#define PRINTLN(...) ((void)0)
#define LOG_ERROR(...) ((void)0)
#define LOG_WARNING(...) ((void)0)
#define LOG_VERBOSE(...) ((void)0)
#define LOG_GET_LEVEL() ((void)0)
#define LOG_SET_LEVEL(l) ((void)0)
#define LOG_SET_OPTION(...) ((void)0)
#define LOG_SET_DELIMITER(...) ((void)0)
#define LOG_ATTACH_SERIAL() ((void)0)
#define LOG_ATTACH_SD(l) ((void)0)
#define LOG_SD_FLUSH() ((void)0)
#define LOG_SD_CLOSE() ((void)0)
#define ASSERT(b) ((void)0)

#else // NDEBUG

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define PRINT(...) DebugLog::Manager::get().print(__VA_ARGS__)
#define PRINTLN(...) DebugLog::Manager::get().println(__VA_ARGS__)
#define LOG_ERROR(...) DebugLog::Manager::get().log(arx::debug::LogLevel::ERRORS, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...) DebugLog::Manager::get().log(arx::debug::LogLevel::WARNINGS, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_VERBOSE(...) DebugLog::Manager::get().log(arx::debug::LogLevel::VERBOSE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_GET_LEVEL() DebugLog::Manager::get().logLevel()
#define LOG_SET_LEVEL(l) DebugLog::Manager::get().logLevel(l)
#define LOG_SET_OPTION(file, line, func) DebugLog::Manager::get().option(file, line, func)
#define LOG_SET_DELIMITER(d) DebugLog::Manager::get().delimiter(d)
#ifdef ARDUINO
    #define LOG_ATTACH_SERIAL(s) DebugLog::Manager::get().attach(s)
    #define LOG_ATTACH_SD(s, p, b, ...) DebugLog::Manager::get().attach(s, p, b, __VA_ARGS__)
    #define LOG_SD_FLUSH() DebugLog::Manager::get().flush()
    #define LOG_SD_CLOSE() DebugLog::Manager::get().close()
    #define ASSERT(b) DebugLog::Manager::get().assertion((b), __FILENAME__, __LINE__, __func__, #b)
#else
    #include <cassert>
    #define ASSERT(b) assert(b)
#endif

#endif // #ifdef NDEBUG


#endif // ARX_DEBUGLOG_H
