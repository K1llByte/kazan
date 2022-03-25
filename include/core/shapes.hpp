#ifndef KZN_SHAPES_HPP
#define KZN_SHAPES_HPP

#include "core/model.hpp"

#include <array>

namespace kzn
{
    // NOTE: Possible shape generator options:
    // - [ ] Clockwise or Counter Clockwise face
    // - [ ] Color
    Model sphere(const float radius, const uint slices, const uint stacks);

    Model cylinder(const float radius, const float height, const uint slices);

    Model box(const float width_x, const float width_y, const float width_z, const uint divisions);

    Model cone(const float radius, const float height, const uint slices, const uint stacks);

    Model plane(const float width);

    Model torus(const float radius, const float cyl_radius, const uint slices, const uint cyl_slices);

    Model icosahedron(const float radius, const uint divisions, bool to_sphere = false);
}

#endif // GENERATOR_H
