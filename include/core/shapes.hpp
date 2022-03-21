#ifndef KZN_SHAPES_HPP
#define KZN_SHAPES_HPP

#include "core/model.hpp"

namespace kzn
{
    class Shape
    {
    public:
        // NOTE: Possible shape generator options:
        // - [ ] Clockwise or Counter Clockwise face
        static Model sphere(const float radius, const uint slices, const uint stacks);

        static Model cylinder(const float radius, const float height, const uint slices);

        static Model box(const float width_x, const float width_y, const float width_z, const uint divisions);

        static Model cone(const float radius, const float height, const uint slices, const uint stacks);

        static Model plane(const float width);

        static Model torus(const float radius, const float cyl_radius, const uint slices, const uint cyl_slices);
    };
}

#endif // GENERATOR_H
