#ifndef UTILS_H
#define UTILS_H

// Queue
#include <cstddef>

#include <array>
#include <stdexcept>

// Debug
#include <iostream>

#include <glm/glm.hpp>

// // Either
// #include <functional>

namespace kzn
{

namespace utils
{

template<typename T, size_t N>
class Queue
{
private:
    
    std::array<T,N> queue;
    size_t front = 0;
    size_t back = 0;
    bool empty = true;

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


// glm::vec3 to_spherical(glm::vec3 coords)
// {
//     const float x = coords.x;
//     const float y = coords.y;
//     const float z = coords.z;
//     return glm::vec3{
//         sqrt(x*x + y*y + z*z),
//         atan2(x,z), // [0, pi]
//         acos(y / sqrt(x*x + y*y + z*z)) // [0, 2pi]
//     };
// }


// glm::vec3 to_cartesian(glm::vec3 coords)
// {
//     const float p = coords.x;
//     const float theta = coords.y;
//     const float phi = coords.z;
//     return glm::vec3{
//         sin(theta) * sin(phi) * p,
//         cos(phi) * p,
//         sin(phi) * p * cos(theta)
//     };
// }


// template<typename First, typename Second>
// class Either
// {
// public:

//     union
//     {
//         First first;
//         Second second;
//     } data;
//     uint8_t contains;

//     Either()
//         : contains{0} {}
//     Either(First&& fst)
//         : data{.first = fst}, contains{1} {}
//     Either(Second&& snd)
//         : data{.second = snd}, contains{2} {}


//     template<typename... Args>
//     constexpr void emplace_first(Args&&... args)
//     {
//         data.first = First(std::forward<First>(args)...);
//     }

//     template<typename... Args>
//     constexpr void emplace_second(Args&&... args)
//     {
//         data.second = Second(std::forward<Second>(args)...);
//     }


//     constexpr bool has_first() const
//     {
//         return contains == 1;
//     }

//     constexpr bool has_second() const
//     {
//         return contains == 2;
//     }


//     First&& first()
//     {
//         return std::move(data.first);
//     }
//     Second&& second()
//     {
//         return std::move(data.second);
//     }

//     void handle(
//         std::function<void(First)> first_handler,
//         std::function<void(Second)> second_handler)
//     {
//         switch(contains)
//         {
//             case 1:
//                 first_handler(data.first);
//                 break;

//             case 2:
//                 second_handler(data.second);
//                 break;

//             default:
//                 break;
//         }
//     }
// };

}

}

#endif // UTILS_H
