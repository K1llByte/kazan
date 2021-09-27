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

    // static std::vector<Model::Vertex> gen_cylinder(const float radius, const float height, const uint slices, const Options opt = {});
};

////////////////////////////////////////////////////////////////


// cg::vnt_vectors gen_cylinder(const float radius, const float height, const uint slices)
// {
// 	invalid_argument_range_exit(radius <= 0 || slices < 3);
	
// 	const size_t NUM_VTX = 12*slices;
//     std::vector<cg::vec3> vertex_list(NUM_VTX);
// 	std::vector<cg::vec3> normals_list(NUM_VTX);
// 	std::vector<cg::vec2> texture_list(NUM_VTX);
//     const float angle = 2*M_PI / slices;
// 	const float half_height = height / 2;

//     const float tex_div_width = 1.0f / slices;
// 	//std::cout << tex_div_width << "\n";

// 	uint it = 0;
// 	uint it_n = 0;
// 	uint it_t = 0;

// 	for(uint i = 0 ; i < slices ; ++i)
// 	{
// 		// Sides
// 		const float tmp1 = i*angle;
// 		const float tmp2 = (i+1)*angle;

// 		// Ensure the calculation is done only once
// 		const float r_cos_tmp1 = radius*cos(tmp1);
// 		const float r_sin_tmp1 = radius*sin(tmp1);
// 		const float r_cos_tmp2 = radius*cos(tmp2);
// 		const float r_sin_tmp2 = radius*sin(tmp2);

// 		// Vertices
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp1 , -half_height , r_sin_tmp1 }; //A
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B
		
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp2 , half_height  , r_sin_tmp2 }; //D
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B	

// 		// Normals
// 		normals_list[it_n++] = cg::vec3{ cos(tmp1) , 0 , sin(tmp1) }; //A
// 		normals_list[it_n++] = cg::vec3{ cos(tmp1) , 0 , sin(tmp1) }; //C
// 		normals_list[it_n++] = cg::vec3{ cos(tmp2) , 0 , sin(tmp2) }; //B
		
// 		normals_list[it_n++] = cg::vec3{ cos(tmp1) , 0 , sin(tmp1) }; //C
// 		normals_list[it_n++] = cg::vec3{ cos(tmp2) , 0 , sin(tmp2) }; //D
// 		normals_list[it_n++] = cg::vec3{ cos(tmp2) , 0 , sin(tmp2) }; //B

// 		// Texture_Coords
// 		texture_list[it_t++] = cg::vec2{ 1.f - i*tex_div_width     , 0.375f }; //A
// 		texture_list[it_t++] = cg::vec2{ 1.f - i*tex_div_width     , 1.0f   }; //C
// 		texture_list[it_t++] = cg::vec2{ 1.f - (i+1)*tex_div_width , 0.375f }; //B
		
// 		texture_list[it_t++] = cg::vec2{ 1.f - i*tex_div_width     , 1.0f   }; //C
// 		texture_list[it_t++] = cg::vec2{ 1.f - (i+1)*tex_div_width , 1.0f   }; //D
// 		texture_list[it_t++] = cg::vec2{ 1.f - (i+1)*tex_div_width , 0.375f }; //B		

// 		// Up Base ///////////////////
		
// 		// Vertices
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp2 , half_height  , r_sin_tmp2 }; //B
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp1 , half_height  , r_sin_tmp1 }; //A
// 		vertex_list[it++] = cg::vec3{ 0          , half_height  , 0          }; //O	

// 		// Normals
// 		normals_list[it_n++] = cg::vec3{ 0 , 1 , 0 }; //B
// 		normals_list[it_n++] = cg::vec3{ 0 , 1 , 0 }; //A
// 		normals_list[it_n++] = cg::vec3{ 0 , 1 , 0 }; //O

// 		// Texture_Coords
// 		texture_list[it_t++] = cg::vec2{ 0.1875f*sin((i+1)*angle) + 0.4375f , 0.1875f*cos((i+1)*angle) + 0.1875f }; //B
// 		texture_list[it_t++] = cg::vec2{ 0.1875f*sin(i*angle) + 0.4375f     , 0.1875f*cos(i*angle) + 0.1875f     }; //A
// 		texture_list[it_t++] = cg::vec2{ 0.4375f                           , 0.1875f                            }; //O

// 		// Down Base ///////////////////

// 		// Vertices
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp1 , -half_height , r_sin_tmp1 }; //A
// 		vertex_list[it++] = cg::vec3{ r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B
// 		vertex_list[it++] = cg::vec3{ 0          , -half_height , 0          }; //O
		
// 		// Normals
// 		normals_list[it_n++] = cg::vec3{ 0 , -1 , 0 }; //B
// 		normals_list[it_n++] = cg::vec3{ 0 , -1 , 0 }; //A
// 		normals_list[it_n++] = cg::vec3{ 0 , -1 , 0 }; //O	

// 		// Texture_Coords
// 		texture_list[it_t++] = cg::vec2{ 0.1875f*sin(i*angle) + 0.8125f     , 0.1875f*cos(i*angle) + 0.1875f     }; //B
// 		texture_list[it_t++] = cg::vec2{ 0.1875f*sin((i+1)*angle) + 0.8125f , 0.1875f*cos((i+1)*angle) + 0.1875f }; //A
// 		texture_list[it_t++] = cg::vec2{ 0.8125f                            , 0.1875f                            }; //O
// 	}

//     return std::make_tuple(vertex_list,normals_list,texture_list);
// }

}

#endif // GENERATOR_H
