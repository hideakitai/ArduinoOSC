#pragma once

#ifndef RINGQUEUE_H
#define RINGQUEUE_H

template<typename T, size_t SIZE>
class RingQueue
{
public:

    inline size_t capacity() const { return SIZE; };
    inline size_t size() const { return (tail_ - head_); };
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
        queue_[(tail_++) % SIZE] = data;
        if      (size() > SIZE) head_++;
    };

    inline const T& front() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % SIZE);
    };
    inline T& front() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + head_ % SIZE);
    };

    inline const T& back() const // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (SIZE + tail_ - 1) % SIZE);
    }
    inline T& back() // throw(Exception)
    {
        // if(empty()) throw Exception();
        return *(queue_ + (SIZE + tail_ - 1) % SIZE);
    }

    inline const T& operator[] (uint8_t index) const
    {
        return *(queue_ + (head_ + index) % SIZE);
    }
    inline T& operator[] (uint8_t index)
    {
        return *(queue_ + (head_ + index) % SIZE);
    }

private:

    volatile size_t head_ {0};
    volatile size_t tail_ {0};
    T queue_[SIZE];
};

#endif
