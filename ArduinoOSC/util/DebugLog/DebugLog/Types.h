#pragma once
#ifndef DEBUGLOG_TYPES_H
#define DEBUGLOG_TYPES_H

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

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    template <typename T>
    using vec_t = std::vector<T>;
    template <typename T>
    using deq_t = std::deque<T>;
    template <typename K, typename V>
    using map_t = std::map<K, V>;
#else   // Do not have libstdc++11
    template <typename T, size_t N>
    using vec_t = arx::vector<T, N>;
    template <typename T, size_t N>
    using deq_t = arx::deque<T, N>;
    template <typename K, typename V, size_t N>
    using map_t = arx::map<K, V, N>;
#endif  // Do not have libstdc++11

// serial loggers
#ifdef ARDUINO
    using string_t = String;
#else
    using string_t = std::string;
#endif

    enum class LogLevel {
        LVL_NONE,
        LVL_ERROR,
        LVL_WARN,
        LVL_INFO,
        LVL_DEBUG,
        LVL_TRACE
    };

    enum class LogBase {
        DEC = 10,
        HEX = 16,
        OCT = 8,
#ifdef ARDUINO
        BIN = 2,
#endif
    };

    template <typename T>
    struct Array {
        T* ptr;
        size_t sz;

    public:
        Array(T* ptr, const size_t sz)
        : ptr(ptr), sz(sz) {}

        const T& operator[](const size_t i) const { return ptr[i]; }
        size_t size() const { return sz; }
    };

    template <typename T>
    inline Array<T> to_arr(T* ptr, const size_t sz) {
        return Array<T>(ptr, sz);
    }

}  // namespace debug
}  // namespace arx

#ifndef DEBUGLOG_DEFAULT_LOG_LEVEL
#define DEBUGLOG_DEFAULT_LOG_LEVEL LogLevel::LVL_INFO
#endif
#ifndef DEBUGLOG_DEFAULT_FILE_LEVEL
#define DEBUGLOG_DEFAULT_FILE_LEVEL LogLevel::LVL_ERROR
#endif

#endif  // DEBUGLOG_TYPES_H
