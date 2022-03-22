#ifndef KZN_SHAPES_HPP
#define KZN_SHAPES_HPP

#include "core/model.hpp"

namespace kzn
{
    // NOTE: Possible shape generator options:
    // - [ ] Clockwise or Counter Clockwise face
    Model sphere(const float radius, const uint slices, const uint stacks);

    Model cylinder(const float radius, const float height, const uint slices);

    Model box(const float width_x, const float width_y, const float width_z, const uint divisions);

    Model cone(const float radius, const float height, const uint slices, const uint stacks);

    Model plane(const float width);

    Model torus(const float radius, const float cyl_radius, const uint slices, const uint cyl_slices);
}

#endif // GENERATOR_H
