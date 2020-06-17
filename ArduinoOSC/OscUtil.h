#pragma once

#ifndef ARDUINOOSC_OSCUTIL_H
#define ARDUINOOSC_OSCUTIL_H

#include <Arduino.h>
#include "OscTypes.h"

namespace arduino {
namespace osc {

    // round to the next multiple of 4, works for size_t and pointer arguments
    template <typename T>
    inline T ceil4(const T& p) { return (T)((size_t(p) + 3) & (~size_t(3))); }

    template <typename T>
    union PodBytes
    {
        T value;
        char bytes[sizeof(T)];
    };

    inline bool isBigEndian()
    {
        const PodBytes<int32_t> p {0x12345678};
        return (p.bytes[0] == 0x12);
    }

    template <typename POD>
    inline POD bytes2pod(const char* bytes)
    {
        PodBytes<POD> p;
        for (size_t i = 0; i < sizeof(POD); ++i)
        {
            if (isBigEndian()) p.bytes[i] = bytes[i];
            else               p.bytes[i] = bytes[sizeof(POD) - i - 1];
        }
        return p.value;
    }

    template <typename POD>
    inline void pod2bytes(const POD& value, char* bytes)
    {
        const PodBytes<POD> p {value};
        for (size_t i = 0; i < sizeof(POD); ++i)
        {
            if (isBigEndian()) bytes[i] = p.bytes[i];
            else               bytes[i] = p.bytes[sizeof(POD) - i - 1];
        }
    }


    inline const char* internalPatternMatch(const char* pattern, const char* path)
    {
        while (*pattern)
        {
            const char *p = pattern;

            if ((*p == '?') && *path)
            {
                ++p;
                ++path;
            }

            // bracketted range, e.g. [a-zABC]
            else if ((*p == '[') && *path)
            {
                ++p;
                bool reverse = false;
                if (*p == '!')
                {
                    reverse = true;
                    ++p;
                }
                bool match = reverse;
                for (; *p && (*p != ']'); ++p)
                {
                    char c0 = *p, c1 = c0;
                    if ((p[1] == '-') && p[2])
                    {
                        p += 2;
                        c1 = *p;
                    }
                    if ((*path >= c0) && (*path <= c1))
                    {
                        match = !reverse;
                    }
                }
                if (!match || (*p != ']'))
                    return pattern;

                ++p;
                ++path;
            }

            // wildcard '*'
            else if (*p == '*')
            {
                while (*p == '*') ++p;

                const char *best = 0;
                while (true)
                {
                    const char *ret = internalPatternMatch(p, path);
                    if (ret && (ret > best))
                        best = ret;
                    if ((*path == 0) || (*path == '/'))
                        break;
                    else
                        ++path;
                }
                return best;
            }

            // the super-wildcard '//'
            else if ((*p == '/') && (*(p + 1) == '/'))
            {
                while (*(p + 1) == '/') ++p;

                const char *best = 0;
                while (true)
                {
                    const char *ret = internalPatternMatch(p, path);
                    if (ret && (ret > best))
                        best = ret;
                    if (*path == 0)
                        break;
                    if ((*path == 0) || ((path = strchr(path+1, '/')) == 0))
                        break;
                }
                return best;
            }

            // braced list {foo,bar,baz}
            else if (*p == '{')
            {
                const char *end = strchr(p, '}'), *q;
                if (!end) return 0; // syntax error in brace list..

                bool match = false;
                do
                {
                    ++p;
                    q = strchr(p, ',');
                    if ((q == 0) || (q > end))
                        q = end;

                    if (strncmp(p, path, q - p) == 0)
                    {
                        path += (q - p);
                        p = end + 1;
                        match = true;
                    }
                    else
                        p = q;
                }
                while ((q != end) && !match);

                if (!match) return pattern;
            }

            // any other character
            else if (*p == *path)
            {
                ++p;
                ++path;
            }

            else
            {
                break;
            }

            pattern = p;
        }

        return (*path == 0 ? pattern : 0);
    }

    inline bool partialPatternMatch(const String &pattern, const String &test)
    {
        const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
        return q != 0;
    }

    inline bool fullPatternMatch(const String &pattern, const String &test)
    {
        const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
        return q && (*q == 0);
    }

    inline bool match(const String& pattern, const String& test, bool full = true)
    {
        if (full) return fullPatternMatch(pattern.c_str(), test.c_str());
        else      return partialPatternMatch(pattern.c_str(), test.c_str());
    }

} // namespace osc
} // namespac arduino


#endif // ARDUINOOSC_OSCUTIL_H
