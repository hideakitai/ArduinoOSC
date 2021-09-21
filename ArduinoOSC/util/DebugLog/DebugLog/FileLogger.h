#pragma once
#ifndef DEBUGLOG_FILE_LOGGER_H
#define DEBUGLOG_FILE_LOGGER_H

#include "Types.h"

namespace arx {
namespace debug {

#ifdef ARDUINO

    struct FileLogger {
        virtual ~FileLogger() {}

        virtual bool is_open() = 0;
        virtual void flush() = 0;

        virtual size_t print(const __FlashStringHelper*) = 0;
        virtual size_t print(const String&) = 0;
        virtual size_t print(const char[]) = 0;
        virtual size_t print(const char) = 0;
        virtual size_t print(const unsigned char, const int = DEC) = 0;
        virtual size_t print(const int, const int = DEC) = 0;
        virtual size_t print(const unsigned int, const int = DEC) = 0;
        virtual size_t print(const long, const int = DEC) = 0;
        virtual size_t print(const unsigned long, const int = DEC) = 0;
        virtual size_t print(const double, const int = 2) = 0;
        virtual size_t print(const Printable&) = 0;

        virtual size_t println(const __FlashStringHelper*) = 0;
        virtual size_t println(const String&) = 0;
        virtual size_t println(const char[]) = 0;
        virtual size_t println(const char) = 0;
        virtual size_t println(const unsigned char, const int = DEC) = 0;
        virtual size_t println(const int, const int = DEC) = 0;
        virtual size_t println(const unsigned int, const int = DEC) = 0;
        virtual size_t println(const long, const int = DEC) = 0;
        virtual size_t println(const unsigned long, const int = DEC) = 0;
        virtual size_t println(const double, const int = 2) = 0;
        virtual size_t println(const Printable&) = 0;
        virtual size_t println(void) = 0;
    };

    template <typename FsType, typename FileType>
    class FsFileLogger : public FileLogger {
        FsType* fs;
        String path;
        FileType file;

    public:
        template <typename FileMode>
        FsFileLogger(FsType& fsclass, const String& path, const FileMode& mode)
        : fs(&fsclass), path(path) {
            file = fs->open(path.c_str(), mode);
        }

        virtual ~FsFileLogger() {
            if (file) file.close();
        }

        virtual bool is_open() override { return file ? true : false; }  // bool file() isn't const...
        virtual void flush() override { file.flush(); }

        virtual size_t print(const __FlashStringHelper* x) override { return file.print(x); }
        virtual size_t print(const String& x) override { return file.print(x); }
        virtual size_t print(const char x[]) override { return file.print(x); }
        virtual size_t print(const char x) override { return file.print(x); }
        virtual size_t print(const unsigned char x, const int b = DEC) override { return file.print(x, b); }
        virtual size_t print(const int x, const int b = DEC) override { return file.print(x, b); }
        virtual size_t print(const unsigned int x, const int b = DEC) override { return file.print(x, b); }
        virtual size_t print(const long x, const int b = DEC) override { return file.print(x, b); }
        virtual size_t print(const unsigned long x, const int b = DEC) override { return file.print(x, b); }
        virtual size_t print(const double x, const int b = 2) override { return file.print(x, b); }
        virtual size_t print(const Printable& x) override { return file.print(x); }

        virtual size_t println(const __FlashStringHelper* x) override { return file.println(x); }
        virtual size_t println(const String& x) override { return file.println(x); }
        virtual size_t println(const char x[]) override { return file.println(x); }
        virtual size_t println(const char x) override { return file.println(x); }
        virtual size_t println(const unsigned char x, const int b = DEC) override { return file.println(x, b); }
        virtual size_t println(const int x, const int b = DEC) override { return file.println(x, b); }
        virtual size_t println(const unsigned int x, const int b = DEC) override { return file.println(x, b); }
        virtual size_t println(const long x, const int b = DEC) override { return file.println(x, b); }
        virtual size_t println(const unsigned long x, const int b = DEC) override { return file.println(x, b); }
        virtual size_t println(const double x, const int b = 2) override { return file.println(x, b); }
        virtual size_t println(const Printable& x) override { return file.println(x); }
        virtual size_t println(void) override { return file.println(); }
    };

#endif  // ARDUINO

}  // namespace debug
}  // namespace arx

#endif  // DEBUGLOG_FILE_LOGGER_H
