#pragma once

#ifndef RINGQUEUE_H
#define RINGQUEUE_H

template<typename T, typename size_type = uint32_t>
class RingQueue
{
public:

    RingQueue(size_type size)
    : size_(size)
    {
        head_ = tail_ = 0;
        queue_ = new T[size_];
    };
    ~RingQueue()  { delete[] queue_; };

    inline size_type capacity() const { return size_; };
    inline size_type size() const { return (tail_ - head_); };
    inline bool empty() const { return tail_ == head_; };
    inline void clear() { head_ = 0; tail_ = 0; };
    inline void pop()
    {
        if (size() == 0) return;
        if (size() == 1) clear();
        else head_++;
    };
    inline void push(T data)
    {
        queue_[(tail_++) % size_] = data;
        if      (size() > size_) head_++;
    };

    inline const T& front() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % size_);
    };
    inline T& front() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % size_);
    };

    inline const T& back() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (size_ + tail_ - 1) % size_);
    }
    inline T& back() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (size_ + tail_ - 1) % size_);
    }

    inline const T& operator[] (uint8_t index) const
    {
        return *(queue_ + (head_ + index) % size_);
    }
    inline T& operator[] (uint8_t index)
    {
        return *(queue_ + (head_ + index) % size_);
    }

private:

    volatile size_type head_;
    volatile size_type tail_;
    const    size_type size_;
    T* queue_;
};

#endif
