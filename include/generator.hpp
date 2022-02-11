#ifndef GENERATOR_H
#define GENERATOR_H

#include "model.hpp"

namespace kzn
{

class ShapeGenerator
{
public:
    struct Options
    {
        const bool clockwise;
    };

    // NOTE: Possible generator options:
    // - [ ] No validation
    // - [x] Clockwise or Counter Clockwise face
    static std::vector<Model::Vertex> gen_sphere(const float radius, const uint slices, const uint stacks, const Options opt = {});

    static std::vector<Model::Vertex> gen_cylinder(const float radius, const float height, const uint slices, const Options opt = {});

    static std::vector<Model::Vertex> gen_box(const float width_x, const float width_y, const float width_z, const uint divisions, const Options opt = {});

    static std::vector<Model::Vertex> gen_cone(const float radius, const float height, const uint slices, const uint stacks, const Options opt = {});

    static std::vector<Model::Vertex> gen_plane(const float width, const Options opt = {});

    static std::vector<Model::Vertex> gen_torus(const float radius, const float cyl_radius, const uint slices, const uint cyl_slices, const Options opt = {});
};

////////////////////////////////////////////////////////////////

}

#endif // GENERATOR_H
