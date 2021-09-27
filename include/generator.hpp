#ifndef GENERATOR_H
#define GENERATOR_H

#include "utils.hpp"
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
    // - No validation
    // - Clockwise or Counter Clockwise face
    static std::vector<Model::Vertex> gen_sphere(const float radius, const uint slices, const uint stacks, const Options opt = {});

    static std::vector<Model::Vertex> gen_cylinder(const float radius, const float height, const uint slices, const Options opt = {});
};

////////////////////////////////////////////////////////////////

}

#endif // GENERATOR_H
