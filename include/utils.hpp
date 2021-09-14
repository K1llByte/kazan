#ifndef UTILS_H
#define UTILS_H

#include <cstddef>

#include <array>
#include <stdexcept>

namespace kzn
{

namespace utils
{

template<typename T, size_t N>
class Queue
{
public:

    bool empty = true;

private:
    
    std::array<T,N> queue;
    size_t front = 0;
    size_t back = 0;

public:

    Queue() = default;
    Queue(const Queue&) = delete;

    constexpr void push(const T& value)
    {
        if(!empty && front == back)
        {
            throw std::runtime_error("Queue is full");
        }

        queue[back] = value;
        back = (back+1) % N;

        empty = false;
    }

    constexpr T pop()
    {
        if(empty)
        {
            throw std::runtime_error("Queue is empty");
        }

        const size_t idx = front;
        front = (front+1) % N;

        if(front == back)
        {
            empty = true;
        }

        return queue[idx];
    }

    bool is_empty() const
    {
        return empty;
    }
};

}

}

#endif // UTILS_H
