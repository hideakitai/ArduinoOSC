#pragma once

#ifndef ARX_TYPE_TRAITS_FUNCTIONAL_H
#define ARX_TYPE_TRAITS_FUNCTIONAL_H

#include <Arduino.h>

#ifdef ARX_TYPE_TRAITS_NEW_DISABLED
    void* operator new (const size_t size, void* ptr) { (void)size; return ptr; }
#else
    #include <new.h>
#endif

#ifdef ARX_TYPE_TRAITS_DISABLED

namespace std {

    // reference:
    // stack overflow https://stackoverflow.com/questions/32074410/stdfunction-bind-like-type-erasure-without-standard-c-library

    template<class Signature>
    struct function;

    template<class R, class... Args>
    class function<R(Args...)>
    {
        struct vtable_t
        {
            void (*mover)(void* src, void* dest);
            void (*destroyer)(void*);
            R (*invoke)(void const* t, Args&&... args);

            template <class T>
            static vtable_t const* get()
            {
                static const vtable_t table =
                {
                    // mover
                    [] (void* src, void* dest)
                    {
                        new(dest) T(move(*static_cast<T*>(src)));
                    },
                    // destroyer
                    [] (void* t)
                    {
                        static_cast<T*>(t)->~T();
                    },
                    // invoke
                    [] (void const* t, Args&&... args) -> R
                    {
                        return (*static_cast<T const*>(t))(std::forward<Args>(args)...);
                    }
                };
                return &table;
            }
        };

        vtable_t const* table {nullptr};
        void* data {nullptr};

    public:

        template <
            class Func,
            class dF = typename std::decay<Func>::type,
            typename enable_if <!std::is_same<dF, function>{}>::type* = nullptr,
            typename enable_if <std::is_convertible<typename result_of<dF&(Args...)>::type, R>{}>::type* = nullptr
        >
        function(const Func& f)
        : table(vtable_t::template get<dF>())
        {
            data = reinterpret_cast<void*>(new Func(f));
        }
        function(const function& o)
        : table(o.table)
        {
            data = o.data;
        }
        function(function&& o)
        : table(o.table)
        {
            if (table) table->mover(o.data, data);
        }
        function()
        {
        }
        ~function()
        {
            if (table) table->destroyer(data);
        }

        function& operator= (const function& o)
        {
            this->~function();
            new(this) function(move(o));
            return *this;
        }
        function& operator= (function&& o)
        {
            this->~function();
            new(this) function(move(o));
            return *this;
        }
        function& operator= (std::nullptr_t p)
        {
            (void)p;
            this->~function();
            return *this;
        }

        explicit operator bool() const
        {
            return table;
        }

        R operator()(Args...args) const
        {
            return table->invoke(data, forward<Args>(args)...);
        }
    };

    template<class R, class... Args>
    inline bool operator== (const function<R(Args...)>& f, std::nullptr_t)
    {
        return !static_cast<bool>(f);
    }

    template<class R, class... Args>
    inline bool operator== (std::nullptr_t, const function<R(Args...)>& f)
    {
        return !static_cast<bool>(f);
    }

    template<class R, class... Args>
    inline bool operator!= (const function<R(Args...)>& f, std::nullptr_t)
    {
        return static_cast<bool>(f);
    }

    template<class R, class... Args>
    inline bool operator!= (std::nullptr_t, const function<R(Args...)>& f)
    {
        return static_cast<bool>(f);
    }

} // namespace std

#endif // ARX_TYPE_TRAITS_DISABLED
#endif // ARX_TYPE_TRAITS_FUNCTIONAL_H
