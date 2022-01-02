#pragma once
#ifndef DEBUGLOG_MANAGER_H
#define DEBUGLOG_MANAGER_H

#include "Types.h"
#include "FileLogger.h"

namespace arx {
namespace debug {

    class Manager {
        LogLevel log_lvl {DEBUGLOG_DEFAULT_LOG_LEVEL};
        LogBase log_base {LogBase::DEC};
        string_t delim {" "};
        bool b_file {true};
        bool b_line {true};
        bool b_func {true};
        bool b_base_reset {true};

#ifdef ARDUINO
        Stream* stream {&Serial};
        FileLogger* logger {nullptr};
        LogLevel file_lvl {DEBUGLOG_DEFAULT_FILE_LEVEL};
        bool b_auto_save {false};
        LogPrecision log_precision {LogPrecision::TWO};
#endif

        // singleton
        Manager() {}
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

    public:
        static Manager& get() {
            static Manager m;
            return m;
        }

        LogLevel log_level() const {
            return log_lvl;
        }

        void log_level(const LogLevel l) {
            log_lvl = l;
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

#ifdef ARDUINO

        ~Manager() {
            if (logger) delete logger;
        }

        void attach(Stream& s) {
            stream = &s;
        }

#ifdef FILE_WRITE
        // TODO: `File` class is always valid for various file systems??
        template <typename FsType, typename FileMode>
        void attach(FsType& s, const String& path, const FileMode& mode, const bool auto_save) {
            close();
            logger = new FsFileLogger<FsType, File>(s, path, mode);
            b_auto_save = auto_save;
        }
#endif

        void assertion(const bool b, const char* file, const int line, const char* func, const char* expr, const String& msg = "") {
            if (!b) {
                string_t str = string_t("[ASSERT] ") + file + string_t(" ") + line + string_t(" ") + func + string_t(" : ") + expr;
                if (msg.length()) str += string_t(" => ") + msg;
                stream->println(str);
                if (logger) {
                    logger->println(str);
                }
                close();
                while (true)
                    ;
            }
        }

        bool is_open() const {
            if (logger) return logger->is_open();
            return false;
        }

        void flush() {
            if (logger) logger->flush();
        }

        void close() {
            flush();
            if (logger) delete logger;
            logger = nullptr;
        }

        LogLevel file_level() const {
            return file_lvl;
        }

        void file_level(const LogLevel l) {
            file_lvl = l;
        }

#endif  // ARDUINO

        template <typename... Args>
        void log(const LogLevel level, const char* file, const int line, const char* func, Args&&... args) {
            bool b_ignore = (log_lvl == LogLevel::LVL_NONE);
#ifdef ARDUINO
            b_ignore &= (file_lvl == LogLevel::LVL_NONE);
#endif
            b_ignore |= (level == LogLevel::LVL_NONE);
            if (b_ignore) return;

            string_t header = generate_header(level, file, line, func);
            if ((int)level <= (int)log_lvl) {
                print(header);  // to avoid delimiter after header
                println(std::forward<Args>(args)...);
            }
#ifdef ARDUINO
            if (!logger) return;
            if ((int)level <= (int)file_lvl) {
                print_file(header);  // to avoid delimiter after header
                println_file(std::forward<Args>(args)...);
            }
#endif
        }

        // ===== print / println =====

        void print() {
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void print(const Head& head, Tail&&... tail) {
#ifdef ARDUINO
            print_one(head, stream);
            if (sizeof...(tail) != 0)
                print_one(delim, stream);
#else
            print_one(head);
            if (sizeof...(tail) != 0)
                print_one(delim);
#endif
            print(std::forward<Tail>(tail)...);
        }

        void println() {
#ifdef ARDUINO
            print_one("\n", stream);
#else
            print_one("\n");
#endif
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename Head, typename... Tail>
        void println(const Head& head, Tail&&... tail) {
#ifdef ARDUINO
            print_one(head, stream);
            if (sizeof...(tail) != 0)
                print_one(delim, stream);
#else
            print_one(head);
            if (sizeof...(tail) != 0)
                print_one(delim);
#endif
            println(std::forward<Tail>(tail)...);
        }

#ifdef ARDUINO
        void print_file() {
            if (!logger) return;
            if (b_auto_save) logger->flush();
            print();
        }

        template <typename Head, typename... Tail>
        void print_file(const Head& head, Tail&&... tail) {
            if (!logger) return;
            print_one(head, logger);
            if (sizeof...(tail) != 0)
                print_one(delim, logger);
            print_file(std::forward<Tail>(tail)...);
        }

        void println_file() {
            if (!logger) return;
            print_one("\n", logger);
            print_file();
        }

        template <typename Head, typename... Tail>
        void println_file(const Head& head, Tail&&... tail) {
            print_one(head, logger);
            if (sizeof...(tail) != 0)
                print_one(delim, logger);
            println_file(std::forward<Tail>(tail)...);
        }
#endif

    private:
#ifdef ARDUINO

        // print without base and precision
        template <typename Head, typename S>
        void print_one(const Head& head, S* s) { s->print(head); }

        // print with base
        template <typename S>
        void print_one(const signed char head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const unsigned char head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const short head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const unsigned short head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const int head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const unsigned int head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const long head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const unsigned long head, S* s) { s->print(head, (int)log_base); }
        template <typename S>
        void print_one(const LogBase& head, S*) {
            log_base = head;
        }

        // print with precision
        template <typename S>
        void print_one(const float head, S* s) { s->print(head, (int)log_precision); }
        template <typename S>
        void print_one(const double head, S* s) { s->print(head, (int)log_precision); }
        template <typename S>
        void print_one(const LogPrecision& head, S*) {
            log_precision = head;
        }

        template <typename S, typename T>
        void print_one(const Array<T>& head, S* s) {
            print_array(head, s);
        }

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11

        template <typename S, typename T>
        void print_one(const vec_t<T>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename T>
        void print_one(const deq_t<T>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename K, typename V>
        void print_one(const map_t<K, V>& head, S* s) {
            print_map(head, s);
        }

#else  // Do not have libstdc++11

        template <typename S, typename T, size_t N>
        void print_one(const vec_t<T, N>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename T, size_t N>
        void print_one(const deq_t<T, N>& head, S* s) {
            print_array(head, s);
        }

        template <typename S, typename K, typename V, size_t N>
        void print_one(const map_t<K, V, N>& head, S* s) {
            print_map(head, s);
        }

#endif  // Do not have libstdc++11

        // print one helper
        template <typename S, typename T>
        void print_array(const T& head, S* s) {
            bool b_base_reset_restore = b_base_reset;
            b_base_reset = false;
            print_one("[", s);
            for (size_t i = 0; i < head.size(); ++i) {
                print_one(head[i], s);
                if (i + 1 != head.size())
                    print_one(", ", s);
            }
            print_one("]", s);
            b_base_reset = b_base_reset_restore;
            if (b_base_reset) log_base = LogBase::DEC;
        }

        template <typename S, typename T>
        void print_map(const T& head, S* s) {
            bool b_base_reset_restore = b_base_reset;
            print_one("{", s);
            const size_t size = head.size();
            size_t i = 0;
            for (const auto& kv : head) {
                print_one(kv.first, s);
                print_one(":", s);
                print_one(kv.second, s);
                if (++i != size)
                    print_one(", ", s);
            }
            print_one("}", s);
            b_base_reset = b_base_reset_restore;
            if (b_base_reset) log_base = LogBase::DEC;
        }

#else

        template <typename Head>
        void print_one(const Head& head) {
            switch (log_base) {
                case LogBase::DEC: std::cout << std::dec; break;
                case LogBase::HEX: std::cout << std::hex; break;
                case LogBase::OCT: std::cout << std::oct; break;
            }
            std::cout << head;
        }

        template <typename T>
        void print_one(const Array<T>& head) {
            print_array(head);
        }

        template <typename T>
        void print_one(const vec_t<T>& head) {
            print_array(head);
        }

        template <typename T>
        void print_one(const deq_t<T>& head) {
            print_array(head);
        }

        template <typename K, typename V>
        void print_one(const map_t<K, V>& head) {
            print_map(head);
        }

        // print one helper
        template <typename T>
        void print_array(const T& head) {
            print_one("[");
            for (size_t i = 0; i < head.size(); ++i) {
                print_one(head[i]);
                if (i + 1 != head.size())
                    print_one(", ");
            }
            print_one("]");
        }

        template <typename T>
        void print_map(const T& head) {
            print_one("{");
            const size_t size = head.size();
            size_t i = 0;
            for (const auto& kv : head) {
                print_one(kv.first);
                print_one(":");
                print_one(kv.second);
                if (++i != size)
                    print_one(", ");
            }
            print_one("}");
        }

#endif

        // ===== other utilities =====

        string_t generate_header(const LogLevel lvl, const char* file, const int line, const char* func) const {
            string_t header;
            switch (lvl) {
                case LogLevel::LVL_ERROR: header = "[ERROR] "; break;
                case LogLevel::LVL_WARN: header = "[WARN] "; break;
                case LogLevel::LVL_INFO: header = "[INFO] "; break;
                case LogLevel::LVL_DEBUG: header = "[DEBUG] "; break;
                case LogLevel::LVL_TRACE: header = "[TRACE] "; break;
                default: header = ""; break;
            }
#ifdef ARDUINO
            if (b_file) header += file + string_t(" ");
            if (b_line) header += string_t("L.") + line + string_t(" ");
            if (b_func) header += func + string_t(" ");
            header += string_t(": ");
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
#endif
            return header;
        }
    };

}  // namespace debug
}  // namespace arx

#endif  // DEBUGLOG_MANAGER_H
