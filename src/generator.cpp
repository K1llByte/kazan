#include "generator.hpp"

namespace kzn
{

std::vector<Model::Vertex> ShapeGenerator::gen_sphere(const float radius, const uint slices, const uint stacks, const Options opt)
{
    if(radius <= 0 || slices < 3 || stacks < 2)
    {
        throw std::runtime_error("invalid generator arguments");
    }

    const size_t NUM_VTX = (6*stacks-6)*slices;
    std::vector<Model::Vertex> vertex_list(NUM_VTX);
    
    const float pi_f = M_PI;
    const float stack_angle = M_PI / stacks;
    const float slices_angle = 2*M_PI / slices;

    const float tex_slice = (1.f / slices);
    const float tex_stacks = (1.f / stacks);

    uint it = 0;
    uint it_n = 0;
    uint it_t = 0;

    using kzn::utils::to_cartesian;
    using kzn::utils::to_spherical;

    for(uint i = 0 ; i < slices ; ++i)
    {	
        // Vertices //////////////////////////////////////////////////////

        // Down Triangle
        vertex_list[it++].position = to_cartesian({ radius , 0                  , pi_f				 });
        vertex_list[it++].position = to_cartesian({ radius , i*slices_angle , pi_f - stack_angle     });
        vertex_list[it++].position = to_cartesian({ radius , (i+1)*slices_angle , pi_f - stack_angle });

        // if constexpr (opt.clockwise)
        // {
        //     const Model::Vertex tmp = vertex_list[it-1];
        //     vertex_list[it-1] = vertex_list[it-2];
        //     vertex_list[it-2] = tmp;
        // }

        // Top Triangle
        vertex_list[it++].position = to_cartesian({ radius , 0			    , 0  		      });
        vertex_list[it++].position = to_cartesian({ radius , (i+1)*slices_angle , stack_angle });
        vertex_list[it++].position = to_cartesian({ radius , i*slices_angle , stack_angle     });

        // Normals //////////////////////////////////////////////////////

        // Down Triangle
        vertex_list[it_n++].normal = to_cartesian({ 1 , 0				   , pi_f			    });
        vertex_list[it_n++].normal = to_cartesian({ 1 , i*slices_angle     , pi_f - stack_angle });
        vertex_list[it_n++].normal = to_cartesian({ 1 , (i+1)*slices_angle , pi_f - stack_angle });

        // Top Triangle
        vertex_list[it_n++].normal = to_cartesian({ 1 , 0			   , 0               });
        vertex_list[it_n++].normal = to_cartesian({ 1 , (i+1)*slices_angle , stack_angle });
        vertex_list[it_n++].normal = to_cartesian({ 1 , i*slices_angle , stack_angle     });

        // Textures //////////////////////////////////////////////////////

        // Down Triangle
        vertex_list[it_t++].uv = { (tex_slice / 2) + i*tex_slice , 0.f        };
        vertex_list[it_t++].uv = { i*tex_slice                   , tex_stacks };
        vertex_list[it_t++].uv = { (i+1)*tex_slice               , tex_stacks };

        // Top Triangle
        vertex_list[it_t++].uv = { (tex_slice / 2) + i*tex_slice , 1.f              };
        vertex_list[it_t++].uv = { (i+1)*tex_slice               , 1.f - tex_stacks };
        vertex_list[it_t++].uv = { i*tex_slice                   , 1.f - tex_stacks };

        // Color //////////////////////////////////////////////////////

        // Down Triangle
        vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };

        // Top Triangle
        vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };

        // Body Squares
        for(uint j = 1 ; j < stacks-1 ; ++j)
        {
            // Vertices //////////////////////////////////////////////////////

            vertex_list[it++].position = to_cartesian({ radius , (i+1)*slices_angle , j*stack_angle     }); //C
            vertex_list[it++].position = to_cartesian({ radius , i*slices_angle     , (j+1)*stack_angle }); //A
            vertex_list[it++].position = to_cartesian({ radius , i*slices_angle     , j*stack_angle     }); //B

            vertex_list[it++].position = to_cartesian({ radius , (i+1)*slices_angle , j*stack_angle     }); //C
            vertex_list[it++].position = to_cartesian({ radius , (i+1)*slices_angle , (j+1)*stack_angle }); //D
            vertex_list[it++].position = to_cartesian({ radius , i*slices_angle     , (j+1)*stack_angle }); //A

            // Normals //////////////////////////////////////////////////////

            vertex_list[it_n++].normal = to_cartesian({ 1 , (i+1)*slices_angle , j*stack_angle     }); //C
            vertex_list[it_n++].normal = to_cartesian({ 1 , i*slices_angle     , (j+1)*stack_angle }); //A
            vertex_list[it_n++].normal = to_cartesian({ 1 , i*slices_angle     , j*stack_angle     }); //B

            vertex_list[it_n++].normal = to_cartesian({ 1 , (i+1)*slices_angle , j*stack_angle     }); //C
            vertex_list[it_n++].normal = to_cartesian({ 1 , (i+1)*slices_angle , (j+1)*stack_angle }); //D
            vertex_list[it_n++].normal = to_cartesian({ 1 , i*slices_angle     , (j+1)*stack_angle }); //A
        
            // Textures //////////////////////////////////////////////////////

            vertex_list[it_t++].uv = { (i+1)*tex_slice , 1.f - j*tex_stacks     }; //C
            vertex_list[it_t++].uv = { i*tex_slice     , 1.f - (j+1)*tex_stacks }; //A
            vertex_list[it_t++].uv = { i*tex_slice     , 1.f - j*tex_stacks     }; //B

            vertex_list[it_t++].uv = { (i+1)*tex_slice , 1.f - j*tex_stacks     }; //C
            vertex_list[it_t++].uv = { (i+1)*tex_slice , 1.f - (j+1)*tex_stacks }; //D
            vertex_list[it_t++].uv = { i*tex_slice     , 1.f - (j+1)*tex_stacks }; //A

            // Color //////////////////////////////////////////////////////

            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };

            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
        }
    }

    return vertex_list;
}


std::vector<Model::Vertex> ShapeGenerator::gen_cylinder(const float radius, const float height, const uint slices, const Options opt)
{
    if(radius <= 0 || slices < 3)
    {
        throw std::runtime_error("invalid generator arguments");
    }
    
    const size_t NUM_VTX = 12*slices;
    std::vector<Model::Vertex> vertex_list(NUM_VTX);
    const float angle = 2*M_PI / slices;
    const float half_height = height / 2;

    const float tex_div_width = 1.0f / slices;
    //std::cout << tex_div_width << "\n";

    uint it = 0;
    uint it_n = 0;
    uint it_t = 0;

    for(uint i = 0 ; i < slices ; ++i)
    {
        // Sides
        const float tmp1 = i*angle;
        const float tmp2 = (i+1)*angle;

        // Ensure the calculation is done only once
        const float r_cos_tmp1 = radius*cos(tmp1);
        const float r_sin_tmp1 = radius*sin(tmp1);
        const float r_cos_tmp2 = radius*cos(tmp2);
        const float r_sin_tmp2 = radius*sin(tmp2);

        // Vertices
        vertex_list[it++].position = { r_cos_tmp1 , -half_height , r_sin_tmp1 }; //A
        vertex_list[it++].position = { r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B
        vertex_list[it++].position = { r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
        
        vertex_list[it++].position = { r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
        vertex_list[it++].position = { r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B	
        vertex_list[it++].position = { r_cos_tmp2 , half_height  , r_sin_tmp2 }; //D

        // Normals
        vertex_list[it_n++].normal = { cos(tmp1) , 0 , sin(tmp1) }; //A
        vertex_list[it_n++].normal = { cos(tmp2) , 0 , sin(tmp2) }; //B
        vertex_list[it_n++].normal = { cos(tmp1) , 0 , sin(tmp1) }; //C
        
        vertex_list[it_n++].normal = { cos(tmp1) , 0 , sin(tmp1) }; //C
        vertex_list[it_n++].normal = { cos(tmp2) , 0 , sin(tmp2) }; //B
        vertex_list[it_n++].normal = { cos(tmp2) , 0 , sin(tmp2) }; //D

        // Texture_Coords
        vertex_list[it_t++].uv = { 1.f - i*tex_div_width     , 0.375f }; //A
        vertex_list[it_t++].uv = { 1.f - (i+1)*tex_div_width , 0.375f }; //B
        vertex_list[it_t++].uv = { 1.f - i*tex_div_width     , 1.0f   }; //C
        
        vertex_list[it_t++].uv = { 1.f - i*tex_div_width     , 1.0f   }; //C
        vertex_list[it_t++].uv = { 1.f - (i+1)*tex_div_width , 0.375f }; //B		
        vertex_list[it_t++].uv = { 1.f - (i+1)*tex_div_width , 1.0f   }; //D

        // Color
        vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };

        vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };

        // Up Base ///////////////////
        
        // Vertices
        vertex_list[it++].position = { r_cos_tmp2 , half_height  , r_sin_tmp2 }; //B
        vertex_list[it++].position = { 0          , half_height  , 0          }; //O	
        vertex_list[it++].position = { r_cos_tmp1 , half_height  , r_sin_tmp1 }; //A

        // Normals
        vertex_list[it_n++].normal = { 0 , 1 , 0 }; //B
        vertex_list[it_n++].normal = { 0 , 1 , 0 }; //O
        vertex_list[it_n++].normal = { 0 , 1 , 0 }; //A

        // Texture_Coords
        vertex_list[it_t++].uv = { 0.1875f*sin((i+1)*angle) + 0.4375f , 0.1875f*cos((i+1)*angle) + 0.1875f }; //B
        vertex_list[it_t++].uv = { 0.4375f                           , 0.1875f                            }; //O
        vertex_list[it_t++].uv = { 0.1875f*sin(i*angle) + 0.4375f     , 0.1875f*cos(i*angle) + 0.1875f     }; //A

        // Color
        vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };

        // Down Base ///////////////////

        // Vertices
        vertex_list[it++].position = { r_cos_tmp1 , -half_height , r_sin_tmp1 }; //A
        vertex_list[it++].position = { 0          , -half_height , 0          }; //O
        vertex_list[it++].position = { r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B
        
        // Normals
        vertex_list[it_n++].normal = { 0 , -1 , 0 }; //B
        vertex_list[it_n++].normal = { 0 , -1 , 0 }; //O	
        vertex_list[it_n++].normal = { 0 , -1 , 0 }; //A

        // Texture_Coords
        vertex_list[it_t++].uv = { 0.1875f*sin(i*angle) + 0.8125f     , 0.1875f*cos(i*angle) + 0.1875f     }; //B
        vertex_list[it_t++].uv = { 0.8125f                            , 0.1875f                            }; //O
        vertex_list[it_t++].uv = { 0.1875f*sin((i+1)*angle) + 0.8125f , 0.1875f*cos((i+1)*angle) + 0.1875f }; //A

        // Color
        vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
    }

    return vertex_list;
}


std::vector<Model::Vertex> ShapeGenerator::gen_box(const float width_x, const float width_y, const float width_z, const uint divisions, const Options opt)
{
    if(width_x <= 0 || width_y <= 0 || width_z <= 0)
    {
        throw std::runtime_error("invalid generator arguments");
    }

    const size_t NUM_VTX = (divisions+1)*(divisions+1)*36;
    std::vector<Model::Vertex> vertex_list(NUM_VTX);

    const float div_width_x = width_x / (divisions+1);
    const float div_width_y = width_y / (divisions+1);
    const float div_width_z = width_z / (divisions+1);
    const float half_width_x = width_x / 2;
    const float half_width_y = width_y / 2;
    const float half_width_z = width_z / 2;

    const float tex_width_x = 1.0f / 4.f;
    const float tex_width_y = 1.0f / 3.f;

    const float tex_div_width_x = tex_width_x / (divisions+1);
    const float tex_div_width_y = tex_width_y / (divisions+1);
    
    uint it = 0;
    uint it_n = 0;
    uint it_t = 0;

    for(uint i = 0 ; i < divisions+1 ; ++i)
    {
        for(uint j = 0 ; j < divisions+1 ; ++j)
        {	
            // Width X
            const float wx1 = half_width_z - div_width_z*i;
            const float wx2 = half_width_z - div_width_z*(i+1);
            // Height X
            const float hx1 = -half_width_y + div_width_y*j;
            const float hx2 = -half_width_y + div_width_y*(j+1);

            // =========== [ X Front Face ] =========== //

            // Vertices
            vertex_list[it++].position = { half_width_x , hx1 , wx1 };
            vertex_list[it++].position = { half_width_x , hx2 , wx2 };
            vertex_list[it++].position = { half_width_x , hx1 , wx2 };

            vertex_list[it++].position = { half_width_x , hx1 , wx1 };
            vertex_list[it++].position = { half_width_x , hx2 , wx1 };
            vertex_list[it++].position = { half_width_x , hx2 , wx2 };

            // Normals
            vertex_list[it_n++].normal = { 1 , 0 , 0 };
            vertex_list[it_n++].normal = { 1 , 0 , 0 };
            vertex_list[it_n++].normal = { 1 , 0 , 0 };

            vertex_list[it_n++].normal = { 1 , 0 , 0 };
            vertex_list[it_n++].normal = { 1 , 0 , 0 };
            vertex_list[it_n++].normal = { 1 , 0 , 0 };

            // Texture_Coord
            vertex_list[it_t++].uv = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };

            vertex_list[it_t++].uv = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };

            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };

            // =========== [ X Back Face ] =========== //

            // Vertices
            vertex_list[it++].position = { -half_width_x , hx1 , wx2 };
            vertex_list[it++].position = { -half_width_x , hx2 , wx2 };
            vertex_list[it++].position = { -half_width_x , hx1 , wx1 };

            vertex_list[it++].position = { -half_width_x , hx2 , wx2 };
            vertex_list[it++].position = { -half_width_x , hx2 , wx1 };
            vertex_list[it++].position = { -half_width_x , hx1 , wx1 };

            // Normals
            vertex_list[it_n++].normal = { -1 , 0 , 0 };
            vertex_list[it_n++].normal = { -1 , 0 , 0 };
            vertex_list[it_n++].normal = { -1 , 0 , 0 };

            vertex_list[it_n++].normal = { -1 , 0 , 0 };
            vertex_list[it_n++].normal = { -1 , 0 , 0 };
            vertex_list[it_n++].normal = { -1 , 0 , 0 };

            // Texture_Coord
            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + j*tex_div_width_y     };

            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + j*tex_div_width_y     };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
    
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };


            // Width Z
            const float wz1_1 = -half_width_x + div_width_x*i;
            const float wz2_1 = -half_width_x + div_width_x*(i+1);
            // Height Z
            const float hz1_1 = -half_width_y + div_width_y*j;
            const float hz2_1 = -half_width_y + div_width_y*(j+1);

            // =========== [ Z Front Face ] =========== //

            // Vertices
            vertex_list[it++].position = { wz2_1 , hz1_1 , half_width_z };
            vertex_list[it++].position = { wz1_1 , hz1_1 , half_width_z };
            vertex_list[it++].position = { wz2_1 , hz2_1 , half_width_z };

            vertex_list[it++].position = { wz1_1 , hz1_1 , half_width_z };
            vertex_list[it++].position = { wz1_1 , hz2_1 , half_width_z };
            vertex_list[it++].position = { wz2_1 , hz2_1 , half_width_z };

            // Normals
            vertex_list[it_n++].normal = { 0 , 0 , 1 };
            vertex_list[it_n++].normal = { 0 , 0 , 1 };
            vertex_list[it_n++].normal = { 0 , 0 , 1 };

            vertex_list[it_n++].normal = { 0 , 0 , 1 };
            vertex_list[it_n++].normal = { 0 , 0 , 1 };
            vertex_list[it_n++].normal = { 0 , 0 , 1 };

            // Texture_Coord
            vertex_list[it_t++].uv = { (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

            vertex_list[it_t++].uv = { i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
    
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };

            // =========== [ Z Back Face ] =========== //
            
            // Width Z
            const float wz1_2 = half_width_x - div_width_x*i;
            const float wz2_2 = half_width_x - div_width_x*(i+1);
            // Height Z
            const float hz1_2 = -half_width_y + div_width_y*j;
            const float hz2_2 = -half_width_y + div_width_y*(j+1);
            
            // Vertices
            vertex_list[it++].position = { wz1_2 , hz1_2 , -half_width_z };
            vertex_list[it++].position = { wz2_2 , hz2_2 , -half_width_z };
            vertex_list[it++].position = { wz2_2 , hz1_2 , -half_width_z };

            vertex_list[it++].position = { wz1_2 , hz1_2 , -half_width_z };
            vertex_list[it++].position = { wz1_2 , hz2_2 , -half_width_z };
            vertex_list[it++].position = { wz2_2 , hz2_2 , -half_width_z };

            // Normals
            vertex_list[it_n++].normal = { 0 , 0 , -1 };
            vertex_list[it_n++].normal = { 0 , 0 , -1 };
            vertex_list[it_n++].normal = { 0 , 0 , -1 };

            vertex_list[it_n++].normal = { 0 , 0 , -1 };
            vertex_list[it_n++].normal = { 0 , 0 , -1 };
            vertex_list[it_n++].normal = { 0 , 0 , -1 };

            // Texture_Coord
            vertex_list[it_t++].uv = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };

            vertex_list[it_t++].uv = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
            vertex_list[it_t++].uv = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
    
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };

            // =========== [ Y Front Face ] =========== //

            // Width Y
            const float wy1_1 = half_width_x - div_width_x*i;
            const float wy2_1 = half_width_x - div_width_x*(i+1);
            // Height Y
            const float hy1_1 = half_width_z - div_width_z*j;
            const float hy2_1 = half_width_z - div_width_z*(j+1);

            // Vertices
            vertex_list[it++].position = { wy1_1 , half_width_y , hy1_1 };
            vertex_list[it++].position = { wy2_1 , half_width_y , hy1_1 };
            vertex_list[it++].position = { wy2_1 , half_width_y , hy2_1 };

            vertex_list[it++].position = { wy1_1 , half_width_y , hy1_1 };
            vertex_list[it++].position = { wy2_1 , half_width_y , hy2_1 };
            vertex_list[it++].position = { wy1_1 , half_width_y , hy2_1 };

            // Normals
            vertex_list[it_n++].normal = { 0 , 1 , 0 };
            vertex_list[it_n++].normal = { 0 , 1 , 0 };
            vertex_list[it_n++].normal = { 0 , 1 , 0 };

            vertex_list[it_n++].normal = { 0 , 1 , 0 };
            vertex_list[it_n++].normal = { 0 , 1 , 0 };
            vertex_list[it_n++].normal = { 0 , 1 , 0 };

            // Texture_Coord
            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y };

            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
    
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
        
            // =========== [ Y Back Face ] =========== //

            // Width Y
            const float wy1_2 = -half_width_x + div_width_x*i;
            const float wy2_2 = -half_width_x + div_width_x*(i+1);
            // Height Y
            const float hy1_2 = half_width_z - div_width_z*j;
            const float hy2_2 = half_width_z - div_width_z*(j+1);

            // Vertices
            vertex_list[it++].position = { wy2_2 , -half_width_y , hy2_2 };
            vertex_list[it++].position = { wy1_2 , -half_width_y , hy1_2 };
            vertex_list[it++].position = { wy2_2 , -half_width_y , hy1_2 };

            vertex_list[it++].position = { wy2_2 , -half_width_y , hy2_2 };
            vertex_list[it++].position = { wy1_2 , -half_width_y , hy2_2 };
            vertex_list[it++].position = { wy1_2 , -half_width_y , hy1_2 };

            // Normals
            vertex_list[it_n++].normal = { 0 , -1 , 0 };
            vertex_list[it_n++].normal = { 0 , -1 , 0 };
            vertex_list[it_n++].normal = { 0 , -1 , 0 };

            vertex_list[it_n++].normal = { 0 , -1 , 0 };
            vertex_list[it_n++].normal = { 0 , -1 , 0 };
            vertex_list[it_n++].normal = { 0 , -1 , 0 };

            // Texture_Coord
            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x , (i+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x     , i*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x     , (i+1)*tex_div_width_y };

            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x , (i+1)*tex_div_width_y };
            vertex_list[it_t++].uv = { 1*tex_width_x + (j+1)*tex_div_width_x , i*tex_div_width_y     };
            vertex_list[it_t++].uv = { 1*tex_width_x + j*tex_div_width_x     , i*tex_div_width_y     };

            // Color
            vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
    
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
        }
    }

    return vertex_list;
}

} // namespace kzn