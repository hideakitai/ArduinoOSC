#pragma once

#ifndef ARX_SMART_PTR_DETAIL_SHARED_COUNT_H
#define ARX_SMART_PTR_DETAIL_SHARED_COUNT_H

#include <Arduino.h>

// imported and simplified from https://github.com/boostorg/smart_ptr

namespace arx
{
    namespace detail
    {
        class sp_counted_base
        {
            sp_counted_base(const sp_counted_base&) = delete;
            sp_counted_base& operator= (const sp_counted_base&) = delete;

            int use_count_;

        public:

            sp_counted_base() : use_count_(1) {}
            virtual ~sp_counted_base() {}

            virtual void dispose() = 0;

            void add_ref()
            {
                ++use_count_;
            }

            void release()
            {
                if (--use_count_ == 0)
                    dispose();
            }

            long use_count() const
            {
                return static_cast<int const volatile &>(use_count_);
            }
        };


        template <typename T>
        class sp_counted_impl : public sp_counted_base
        {
            T* ptr;

            sp_counted_impl(const sp_counted_impl&) = delete;
            sp_counted_impl& operator= (const sp_counted_impl&) = delete;

            typedef sp_counted_impl<T> this_type;

        public:

            sp_counted_impl(T* ptr) : ptr(ptr) {}
            virtual ~sp_counted_impl() {}

            virtual void dispose() override
            {
                delete ptr;
            }
        };



        class shared_count
        {
        private:

            sp_counted_base* pi_;

        public:

            shared_count() : pi_(0)
            {
            }

            explicit shared_count(sp_counted_base* p) : pi_(p)
            {
            }

            template<class Y>
            explicit shared_count(Y* p) : pi_(0)
            {
                pi_ = new sp_counted_impl<Y>(p);

                if(pi_ == 0)
                {
                    delete p;
                }
            }

            ~shared_count()
            {
                if(pi_ != 0) pi_->release();
            }

            shared_count(const shared_count& r) : pi_(r.pi_)
            {
                if(pi_ != 0) pi_->add_ref();
            }

            shared_count(shared_count&& r) : pi_(r.pi_)
            {
                r.pi_ = 0;
            }

            shared_count& operator= (const shared_count& r)
            {
                sp_counted_base* tmp = r.pi_;
                if(tmp != pi_)
                {
                    if(tmp != 0) tmp->add_ref();
                    if(pi_ != 0) pi_->release();
                    pi_ = tmp;
                }
                return *this;
            }

            void swap(shared_count& r)
            {
                sp_counted_base* tmp = r.pi_;
                r.pi_ = pi_;
                pi_ = tmp;
            }

            long use_count() const
            {
                return (pi_ != 0) ? pi_->use_count() : 0;
            }

            bool unique() const
            {
                return use_count() == 1;
            }

            bool empty() const
            {
                return pi_ == 0;
            }

            friend inline bool operator== (const shared_count& a, const shared_count& b)
            {
                return a.pi_ == b.pi_;
            }
        };

    } // namespace detail

} // namespace arx



#endif // ARX_SMART_PTR_DETAIL_SHARED_COUNT_H
