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

#ifdef ARDUINO
#ifdef DEC
#undef DEC
static constexpr uint8_t DEC {10};
#endif
#ifdef HEX
#undef HEX
static constexpr uint8_t HEX {16};
#endif
#ifdef OCT
#undef OCT
static constexpr uint8_t OCT {8};
#endif
#ifdef BIN
#undef BIN
static constexpr uint8_t BIN {2};
#endif
#endif

namespace arx {
namespace debug {

    namespace detail {
        template <class T>
        struct remove_reference { using type = T; };
        template <class T>
        struct remove_reference<T&> { using type = T; };
        template <class T>
        struct remove_reference<T&&> { using type = T; };
        template <class T>
        constexpr T&& forward(typename remove_reference<T>::type& t) noexcept { return static_cast<T&&>(t); }
        template <class T>
        constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept { return static_cast<T&&>(t); }
    }  // namespace detail

// serial loggers
#ifdef ARDUINO
    using string_t = String;
#else
    using string_t = std::string;
#endif

// sd loggers
#ifdef ARDUINO

    struct Logger {
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
    class SdLogger : public Logger {
        SdType* sd;
        String path;
        FileType file;

    public:
        SdLogger(SdType& sdclass, const String& path)
        : sd(&sdclass), path(path) {
#ifdef O_WRITE
            file = sd->open(path.c_str(), O_WRITE | O_CREAT);
#elif defined(FILE_WRITE)
            file = sd->open(path.c_str(), FILE_WRITE);
#endif
        }

        virtual ~SdLogger() {
            if (file) file.close();
        }
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

#endif  // sd loggers

    enum class LogLevel {
        NONE,
        ERRORS,
        WARNINGS,
        VERBOSE
    };

    enum class LogBase {
        DEC = 10,
        HEX = 16,
        OCT = 8,
#ifdef ARDUINO
        BIN = 2,
#endif
    };

#ifndef DEBUGLOG_DEFAULT_LOGLEVEL
#define DEBUGLOG_DEFAULT_LOGLEVEL LogLevel::VERBOSE
#endif

    class Manager {
#ifdef ARDUINO
        Stream* stream {&Serial};
        Logger* logger {nullptr};
#endif
        LogLevel log_level {DEBUGLOG_DEFAULT_LOGLEVEL};
        LogBase log_base {LogBase::DEC};
        string_t delim {" "};
        bool b_file {true};
        bool b_line {true};
        bool b_func {true};
        bool b_base_reset {true};
#ifdef ARDUINO
        bool b_auto_save {false};
        bool b_only_sd {false};
#endif

        // singleton
        Manager() {}
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

    public:
        ~Manager() {
#ifdef ARDUINO
            if (logger) delete logger;
#endif
        }

        static Manager& get() {
            static Manager m;
            return m;
        }

#ifdef ARDUINO
        void attach(Stream& s) { stream = &s; }

        void flush() {
            if (logger) logger->flush();
        }

        void close() {
            flush();
            if (logger) delete logger;
        }

#ifdef FILE_WRITE
        template <typename SdType>
        void attach(SdType& s, const String& path, bool auto_save, bool only_sd = false) {
            close();
            logger = new SdLogger<SdType, File>(s, path);
            b_auto_save = auto_save;
            b_only_sd = only_sd;
        }
#endif

#endif  // ARDUINO

        void print() {
#ifdef ARDUINO
            if (logger && b_auto_save) logger->flush();
#endif
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void print(Head&& head, Tail&&... tail) {
            print_impl(head, sizeof...(tail));
#ifdef ARDUINO
            print(detail::forward<Tail>(tail)...);
#else
            print(std::forward<Tail>(tail)...);
#endif
        }

        void println() {
#ifdef ARDUINO
            if (!b_only_sd) {
                stream->println();
            }
            if (logger) {
                logger->println();
                if (b_auto_save) logger->flush();
            }
#else
            std::cout << std::endl;
#endif
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void println(Head&& head, Tail&&... tail) {
            print_impl(head, sizeof...(tail));
#ifdef ARDUINO
            println(detail::forward<Tail>(tail)...);
#else
            println(std::forward<Tail>(tail)...);
#endif
        }

#ifdef ARDUINO
        void assertion(bool b, const char* file, int line, const char* func, const char* expr) {
            while (!b) {
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
        void log(LogLevel level, const char* file, int line, const char* func, Args&&... args) {
            if ((log_level == LogLevel::NONE) || (level == LogLevel::NONE)) return;
            if ((int)level <= (int)log_level) {
                string_t lvl_str;
                switch (level) {
                    case LogLevel::ERRORS:
                        lvl_str = "[ERROR] ";
                        break;
                    case LogLevel::WARNINGS:
                        lvl_str = "[WARNING] ";
                        break;
                    case LogLevel::VERBOSE:
                        lvl_str = "[VERBOSE] ";
                        break;
                    default:
                        lvl_str = "";
                        break;
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
                if (b_file) {
                    header += file;
                    header += " ";
                };
                if (b_line) {
                    header += "L.";
                    header += std::to_string(line);
                    header += " ";
                }
                if (b_func) {
                    header += func;
                    header += " ";
                };
                header += ": ";
                print(header);
                println(std::forward<Args>(args)...);
#endif
            }
        }

        void option(const bool en_file, const bool en_line, const bool en_func) {
            b_file = en_file;
            b_line = en_line;
            b_func = en_func;
        }

        void delimiter(const string_t& del) {
            delim = del;
        }

        void base_reset(const bool b) {
            b_base_reset = b;
        }

    private:
        template <typename Head>
        void print_impl(Head&& head, const size_t size) {
#ifdef ARDUINO
            if (!b_only_sd) {
                print_exec(head, stream);
                if (size != 0)
                    stream->print(delim);
            }
            if (logger) {
                print_exec(head, logger);
                if (size != 0)
                    logger->print(delim);
            }
#else
            print_exec(head);
            if (size != 0)
                std::cout << delim;
#endif
        }

        void print_impl(LogBase& head, const size_t) {
            log_base = head;
        }

#ifdef ARDUINO
        template <typename Head, typename S>
        void print_exec(Head&& head, S* s) { s->print(head); }
        template <typename S>
        void print_exec(signed char head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(unsigned char head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(short head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(unsigned short head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(int head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(unsigned int head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(long head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_exec(unsigned long head, S* s) { s->print(head, (int)log_base); }
#else
        template <typename Head>
        void print_exec(Head&& head) {
            switch (log_base) {
                case LogBase::DEC: std::cout << std::dec; break;
                case LogBase::HEX: std::cout << std::hex; break;
                case LogBase::OCT: std::cout << std::oct; break;
            }
            std::cout << head;
        }
#endif
    };

}  // namespace debug
}  // namespace arx

namespace DebugLog = arx::debug;
using DebugLogLevel = arx::debug::LogLevel;
using DebugLogBase = arx::debug::LogBase;

// PRINT and PRINTLN are always enabled regardless of debug mode or release mode
#define PRINT(...) DebugLog::Manager::get().print(__VA_ARGS__)
#define PRINTLN(...) DebugLog::Manager::get().println(__VA_ARGS__)
#define LOG_GET_LEVEL() DebugLog::Manager::get().logLevel()
#define LOG_SET_LEVEL(l) DebugLog::Manager::get().logLevel(l)
#define LOG_SET_OPTION(file, line, func) DebugLog::Manager::get().option(file, line, func)
#define LOG_SET_DELIMITER(d) DebugLog::Manager::get().delimiter(d)
#define LOG_SET_BASE_RESET(b) DebugLog::Manager::get().base_reset(b)
#ifdef ARDUINO
#define LOG_ATTACH_SERIAL(s) DebugLog::Manager::get().attach(s)
#define LOG_ATTACH_SD(s, p, b, ...) DebugLog::Manager::get().attach(s, p, b, __VA_ARGS__)
#endif  // ARDUINO

#include "DebugLogRestoreState.h"

#endif  // ARX_DEBUGLOG_H
