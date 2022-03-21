#ifndef KZN_MATH_COORDINATES_HPP
#define KZN_MATH_COORDINATES_HPP

namespace kzn::math
{
    constexpr glm::vec3 cartesian_to_spherical(glm::vec3 coords);
    constexpr glm::vec3 spherical_to_cartesian(glm::vec3 coords);
    constexpr glm::vec3 cylindrical_to_cartesian(glm::vec3 coords);
    constexpr glm::vec3 cartesian_to_cylindrical(glm::vec3 coords);

    //////////////////////////////////////////////
    
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
    
    
    constexpr glm::vec3 cylindrical_to_cartesian(glm::vec3 coords)
    {
        const float s_radius = coords.x;
        const float y = coords.y;
        const float s_theta = coords.z;
        return glm::vec3(s_radius*sin(s_theta) , y , s_radius*cos(s_theta));
    }
    
    
    constexpr glm::vec3 cartesian_to_cylindrical(glm::vec3 coords)
    {
        const float x = coords.x;
        const float y = coords.y;
        const float z = coords.z;
        return { sqrt(x*x + z*z) , y , atan2(x,z) };
    }
} // namespace kzn::math

#endif // KZN_MATH_COORDINATES_HPP