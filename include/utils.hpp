#ifndef UTILS_H
#define UTILS_H

// Queue
#include <cstddef>

#include <array>
#include <stdexcept>
#include <functional>

// Debug
#include <iostream>

#include <glm/glm.hpp>

// // Either
// #include <functional>

namespace kzn
{

namespace utils
{

template<typename T>
constexpr size_t multiple_hash(size_t seed, const T& head)
{
    const size_t this_seed = std::hash<T>{}(head) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed ^ this_seed;
}

template<typename T, typename... Args>
constexpr size_t multiple_hash(size_t seed, const T& head, const Args&... tail)
{
    const size_t this_seed = std::hash<T>{}(head) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    const size_t tail_seed = multiple_hash(this_seed, tail...);
    return seed ^ tail_seed;
}


constexpr float radians(const float degrees)
{
    return degrees * M_PI / 180.f;
}


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


constexpr glm::vec3 cartesian_to_spherical(glm::vec3 coords)
{
    const float x = coords.x;
    const float y = coords.y;
    const float z = coords.z;
    return glm::vec3{
        sqrt(x*x + y*y + z*z),
        atan2(x,z), // [0, pi]
        acos(y / sqrt(x*x + y*y + z*z)) // [0, 2pi]
    };
}


constexpr glm::vec3 spherical_to_cartesian(glm::vec3 coords)
{
    const float p = coords.x;
    const float theta = coords.y;
    const float phi = coords.z;
    return glm::vec3{
        sin(theta) * sin(phi) * p,
        cos(phi) * p,
        sin(phi) * p * cos(theta)
    };
}


constexpr glm::vec3 polar_to_cartesian(glm::vec3 coords)
{
    const float s_radius = coords.x;
    const float y = coords.y;
    const float s_theta = coords.z;
    return glm::vec3(s_radius*sin(s_theta) , y , s_radius*cos(s_theta));
}


constexpr glm::vec3 cartesian_to_polar(glm::vec3 coords)
{
    const float x = coords.x;
    const float y = coords.y;
    const float z = coords.z;
    return { sqrt(x*x + z*z) , y , atan2(x,z) };
}


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
