#include "core/shapes.hpp"

#include "math/coordinates.hpp"
#include "math/constants.hpp"

#include <stdexcept>

using kzn::math::spherical_to_cartesian;
using kzn::math::cylindrical_to_cartesian;

namespace kzn
{
    Model sphere(const float radius, const uint slices, const uint stacks)
    {
        if(radius <= 0 || slices < 3 || stacks < 2)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        const size_t NUM_VTX = (6*stacks-6)*slices;
        std::vector<Vertex> vertex_list(NUM_VTX);

        const float pi_f = PI;
        const float stack_angle = PI / stacks;
        const float slices_angle = 2*PI / slices;

        const float tex_slice = (1.f / slices);
        const float tex_stacks = (1.f / stacks);

        uint it = 0;
        uint it_n = 0;
        uint it_t = 0;


        for(size_t i = 0 ; i < slices ; ++i)
        {	
            // Vertices //////////////////////////////////////////////////////

            // Down Triangle
            vertex_list[it++].position = spherical_to_cartesian({ radius , 0                  , pi_f				 });
            vertex_list[it++].position = spherical_to_cartesian({ radius , i*slices_angle , pi_f - stack_angle     });
            vertex_list[it++].position = spherical_to_cartesian({ radius , (i+1)*slices_angle , pi_f - stack_angle });

            // Top Triangle
            vertex_list[it++].position = spherical_to_cartesian({ radius , 0			    , 0  		      });
            vertex_list[it++].position = spherical_to_cartesian({ radius , (i+1)*slices_angle , stack_angle });
            vertex_list[it++].position = spherical_to_cartesian({ radius , i*slices_angle , stack_angle     });

            // Normals //////////////////////////////////////////////////////

            // Down Triangle
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , 0				   , pi_f			    });
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , i*slices_angle     , pi_f - stack_angle });
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , (i+1)*slices_angle , pi_f - stack_angle });

            // Top Triangle
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , 0			   , 0               });
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , (i+1)*slices_angle , stack_angle });
            vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , i*slices_angle , stack_angle     });

            // Textures //////////////////////////////////////////////////////

            // Down Triangle
            vertex_list[it_t++].tex_coords = { (tex_slice / 2) + i*tex_slice , 0.f        };
            vertex_list[it_t++].tex_coords = { i*tex_slice                   , tex_stacks };
            vertex_list[it_t++].tex_coords = { (i+1)*tex_slice               , tex_stacks };

            // Top Triangle
            vertex_list[it_t++].tex_coords = { (tex_slice / 2) + i*tex_slice , 1.f              };
            vertex_list[it_t++].tex_coords = { (i+1)*tex_slice               , 1.f - tex_stacks };
            vertex_list[it_t++].tex_coords = { i*tex_slice                   , 1.f - tex_stacks };

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

                vertex_list[it++].position = spherical_to_cartesian({ radius , (i+1)*slices_angle , j*stack_angle     }); //C
                vertex_list[it++].position = spherical_to_cartesian({ radius , i*slices_angle     , (j+1)*stack_angle }); //A
                vertex_list[it++].position = spherical_to_cartesian({ radius , i*slices_angle     , j*stack_angle     }); //B

                vertex_list[it++].position = spherical_to_cartesian({ radius , (i+1)*slices_angle , j*stack_angle     }); //C
                vertex_list[it++].position = spherical_to_cartesian({ radius , (i+1)*slices_angle , (j+1)*stack_angle }); //D
                vertex_list[it++].position = spherical_to_cartesian({ radius , i*slices_angle     , (j+1)*stack_angle }); //A

                // Normals //////////////////////////////////////////////////////

                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , (i+1)*slices_angle , j*stack_angle     }); //C
                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , i*slices_angle     , (j+1)*stack_angle }); //A
                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , i*slices_angle     , j*stack_angle     }); //B

                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , (i+1)*slices_angle , j*stack_angle     }); //C
                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , (i+1)*slices_angle , (j+1)*stack_angle }); //D
                vertex_list[it_n++].normal = spherical_to_cartesian({ 1 , i*slices_angle     , (j+1)*stack_angle }); //A
            
                // Textures //////////////////////////////////////////////////////

                vertex_list[it_t++].tex_coords = { (i+1)*tex_slice , 1.f - j*tex_stacks     }; //C
                vertex_list[it_t++].tex_coords = { i*tex_slice     , 1.f - (j+1)*tex_stacks }; //A
                vertex_list[it_t++].tex_coords = { i*tex_slice     , 1.f - j*tex_stacks     }; //B

                vertex_list[it_t++].tex_coords = { (i+1)*tex_slice , 1.f - j*tex_stacks     }; //C
                vertex_list[it_t++].tex_coords = { (i+1)*tex_slice , 1.f - (j+1)*tex_stacks }; //D
                vertex_list[it_t++].tex_coords = { i*tex_slice     , 1.f - (j+1)*tex_stacks }; //A

                // Color //////////////////////////////////////////////////////

                vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };

                vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            }
        }

        return Model(std::move(vertex_list));
    }


    Model cylinder(const float radius, const float height, const uint slices)
    {
        if(radius <= 0 || slices < 3)
        {
            throw std::runtime_error("invalid shape arguments");
        }
        
        const size_t NUM_VTX = 12*slices;
        std::vector<Vertex> vertex_list(NUM_VTX);
        const float angle = 2*PI / static_cast<float>(slices);
        const float half_height = height / 2;

        const float tex_div_width = 1.0f / static_cast<float>(slices);
        //std::cout << tex_div_width << "\n";

        uint it = 0;
        uint it_n = 0;
        uint it_t = 0;

        for(size_t i = 0 ; i < slices ; ++i)
        {
            // Sides
            const float tmp1 = i*angle;
            const float tmp2 = (i+1)*angle;

            // Ensure the calculation is done only once
            const float r_cos_tmp1 = radius*std::cos(tmp1);
            const float r_sin_tmp1 = radius*std::sin(tmp1);
            const float r_cos_tmp2 = radius*std::cos(tmp2);
            const float r_sin_tmp2 = radius*std::sin(tmp2);

            // Vertices
            vertex_list[it++].position = { r_cos_tmp1 , -half_height , r_sin_tmp1 }; //A
            vertex_list[it++].position = { r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B
            vertex_list[it++].position = { r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
            
            vertex_list[it++].position = { r_cos_tmp1 , half_height  , r_sin_tmp1 }; //C
            vertex_list[it++].position = { r_cos_tmp2 , -half_height , r_sin_tmp2 }; //B	
            vertex_list[it++].position = { r_cos_tmp2 , half_height  , r_sin_tmp2 }; //D

            // Normals
            vertex_list[it_n++].normal = { std::cos(tmp1) , 0 , std::sin(tmp1) }; //A
            vertex_list[it_n++].normal = { std::cos(tmp2) , 0 , std::sin(tmp2) }; //B
            vertex_list[it_n++].normal = { std::cos(tmp1) , 0 , std::sin(tmp1) }; //C
            
            vertex_list[it_n++].normal = { std::cos(tmp1) , 0 , std::sin(tmp1) }; //C
            vertex_list[it_n++].normal = { std::cos(tmp2) , 0 , std::sin(tmp2) }; //B
            vertex_list[it_n++].normal = { std::cos(tmp2) , 0 , std::sin(tmp2) }; //D

            // Texture_Coords
            vertex_list[it_t++].tex_coords = { 1.f - i*tex_div_width     , 0.375f }; //A
            vertex_list[it_t++].tex_coords = { 1.f - (i+1)*tex_div_width , 0.375f }; //B
            vertex_list[it_t++].tex_coords = { 1.f - i*tex_div_width     , 1.0f   }; //C
            
            vertex_list[it_t++].tex_coords = { 1.f - i*tex_div_width     , 1.0f   }; //C
            vertex_list[it_t++].tex_coords = { 1.f - (i+1)*tex_div_width , 0.375f }; //B		
            vertex_list[it_t++].tex_coords = { 1.f - (i+1)*tex_div_width , 1.0f   }; //D

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
            vertex_list[it_t++].tex_coords = { 0.1875f*std::sin((i+1)*angle) + 0.4375f , 0.1875f*std::cos((i+1)*angle) + 0.1875f }; //B
            vertex_list[it_t++].tex_coords = { 0.4375f                           , 0.1875f                            }; //O
            vertex_list[it_t++].tex_coords = { 0.1875f*std::sin(i*angle) + 0.4375f     , 0.1875f*std::cos(i*angle) + 0.1875f     }; //A

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
            vertex_list[it_t++].tex_coords = { 0.1875f*std::sin(i*angle) + 0.8125f     , 0.1875f*std::cos(i*angle) + 0.1875f     }; //B
            vertex_list[it_t++].tex_coords = { 0.8125f                            , 0.1875f                            }; //O
            vertex_list[it_t++].tex_coords = { 0.1875f*std::sin((i+1)*angle) + 0.8125f , 0.1875f*std::cos((i+1)*angle) + 0.1875f }; //A

            // Color
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
        }

        return Model(std::move(vertex_list));
    }


    Model box(const float width_x, const float width_y, const float width_z, const uint divisions)
    {
        if(width_x <= 0 || width_y <= 0 || width_z <= 0)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        const size_t NUM_VTX = (divisions+1)*(divisions+1)*36;
        std::vector<Vertex> vertex_list(NUM_VTX);

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

        for(size_t i = 0 ; i < divisions+1 ; ++i)
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
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };

                vertex_list[it_t++].tex_coords = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

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
                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + j*tex_div_width_y     };

                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - (i+1)*tex_div_width_x) , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 3*tex_width_x + (tex_width_x - i*tex_div_width_x    ) , 1*tex_width_y + j*tex_div_width_y     };

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
                vertex_list[it_t++].tex_coords = { (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

                vertex_list[it_t++].tex_coords = { i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

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
                vertex_list[it_t++].tex_coords = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + j*tex_div_width_y     };

                vertex_list[it_t++].tex_coords = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + j*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 2*tex_width_x + i*tex_div_width_x     , 1*tex_width_y + (j+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 2*tex_width_x + (i+1)*tex_div_width_x , 1*tex_width_y + (j+1)*tex_div_width_y };

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
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y };

                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x , 2*tex_width_y + (i+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x     , 2*tex_width_y + i*tex_div_width_y };

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
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x , (i+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x     , i*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x     , (i+1)*tex_div_width_y };

                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x , (i+1)*tex_div_width_y };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + (j+1)*tex_div_width_x , i*tex_div_width_y     };
                vertex_list[it_t++].tex_coords = { 1*tex_width_x + j*tex_div_width_x     , i*tex_div_width_y     };

                // Color
                vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
        
                vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            }
        }

        return Model(std::move(vertex_list));
    }


    Model cone(const float radius, const float height, const uint slices, const uint stacks)
    {
        if(radius <= 0 || height < 0 || slices < 3 || stacks < 1)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        const size_t NUM_VTX = 6*slices*stacks;
        std::vector<Vertex> vertex_list(NUM_VTX);
        const float angle = 2*PI / slices;
        const float stack_height = height / stacks;
        const float stack_radius = radius / stacks;

        const float tex_div_slice = 2*PI / slices;
        

        uint it = 0;
        uint it_n = 0;
        uint it_t = 0;

        for(size_t i = 0 ; i < slices ; ++i)
        {
            const float a1 = i*angle;
            const float a2 = (i+1)*angle;

            // Top Triangle
            
            // Vertices
            vertex_list[it++].position = { 				    	 0 , 				height ,  0 }; //A
            vertex_list[it++].position = cylindrical_to_cartesian({ stack_radius , height - stack_height , a2 }); //B
            vertex_list[it++].position = cylindrical_to_cartesian({ stack_radius , height - stack_height , a1 }); //C

            // Normals
            vertex_list[it_n++].normal =  cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , (a1 + a2)/2 }); //A
            vertex_list[it_n++].normal =  cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) ,          a2 }); //B
            vertex_list[it_n++].normal =  cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) ,          a1 }); //C

            // Texture_Coords
            vertex_list[it_t++].tex_coords = { 0.75f , 0.5f }; //A
            vertex_list[it_t++].tex_coords = { 0.75f + (0.2f / stacks)*std::cos((i+1)*tex_div_slice) , 0.5f + (0.5f / stacks)*std::sin((i+1)*tex_div_slice) }; //B
            vertex_list[it_t++].tex_coords = { 0.75f + (0.2f / stacks)*std::cos(    i*tex_div_slice) , 0.5f + (0.5f / stacks)*std::sin(    i*tex_div_slice) }; //C

            // Color
            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };

            // Cone Body
            for(uint j = 0 ; j < stacks-1 ; ++j)
            {
                // Ensure the calculation is done only once
                const float r1 = radius - j*stack_radius;
                const float r2 = radius - (j+1)*stack_radius;
                const float y1 = j*stack_height;
                const float y2 = (j+1)*stack_height;

                // Vertices
                vertex_list[it++].position = cylindrical_to_cartesian({ r1 , y1 , a1 }); //A
                vertex_list[it++].position = cylindrical_to_cartesian({ r2 , y2 , a1 }); //C
                vertex_list[it++].position = cylindrical_to_cartesian({ r2 , y2 , a2 }); //B

                vertex_list[it++].position = cylindrical_to_cartesian({ r1 , y1 , a2 }); //D
                vertex_list[it++].position = cylindrical_to_cartesian({ r1 , y1 , a1 }); //A
                vertex_list[it++].position = cylindrical_to_cartesian({ r2 , y2 , a2 }); //B

                // Normals
                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a1 }) ); //A
                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a1 }) ); //C
                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a2 }) ); //B

                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a2 }) ); //D
                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a1 }) ); //A
                vertex_list[it_n++].normal = glm::normalize( cylindrical_to_cartesian({ 1 , std::sin(atan(radius/height)) , a2 }) ); //B

                // Texture_Coords
                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-    j)*(0.25f / stacks)*std::cos(    i*tex_div_slice) , 0.5f + (stacks-    j)*(0.5f / stacks)*std::sin(    i*tex_div_slice) }; //A
                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-(j+1))*(0.25f / stacks)*std::cos(    i*tex_div_slice) , 0.5f + (stacks-(j+1))*(0.5f / stacks)*std::sin(    i*tex_div_slice) }; //B
                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-(j+1))*(0.25f / stacks)*std::cos((i+1)*tex_div_slice) , 0.5f + (stacks-(j+1))*(0.5f / stacks)*std::sin((i+1)*tex_div_slice) }; //C

                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-    j)*(0.25f / stacks)*std::cos((i+1)*tex_div_slice) , 0.5f + (stacks-    j)*(0.5f / stacks)*std::sin((i+1)*tex_div_slice) }; //D
                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-    j)*(0.25f / stacks)*std::cos(    i*tex_div_slice) , 0.5f + (stacks-    j)*(0.5f / stacks)*std::sin(    i*tex_div_slice) }; //B
                vertex_list[it_t++].tex_coords = { 0.75f + (stacks-(j+1))*(0.25f / stacks)*std::cos((i+1)*tex_div_slice) , 0.5f + (stacks-(j+1))*(0.5f / stacks)*std::sin((i+1)*tex_div_slice) }; //A

                // Color
                vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };

                vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            }

            //Base Triangle
            vertex_list[it++].position = cylindrical_to_cartesian({ radius , 0 , a2 }); //B
            vertex_list[it++].position = cylindrical_to_cartesian({      0 , 0 ,  0 }); //O
            vertex_list[it++].position = cylindrical_to_cartesian({ radius , 0 , a1 }); //A

            // Normals
            vertex_list[it_n++].normal = { 0 , -1 , 0 }; //B
            vertex_list[it_n++].normal = { 0 , -1 , 0 }; //A
            vertex_list[it_n++].normal = { 0 , -1 , 0 }; //O

            // Texture_Coords
            vertex_list[it_t++].tex_coords = { 0.25f + 0.25f*std::cos(i*tex_div_slice) , 0.5f + 0.5f*std::sin(i*tex_div_slice) }; //A
            vertex_list[it_t++].tex_coords = { 0.25f , 0.5f }; //B
            vertex_list[it_t++].tex_coords = { 0.25f + 0.25f*std::cos((i+1)*tex_div_slice) , 0.5f + 0.5f*std::sin((i+1)*tex_div_slice) }; //C

            vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
            vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
        }

        return Model(std::move(vertex_list));
    }


    Model plane(const float width)
    {
        if(width <= 0)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        std::vector<Vertex> vertex_list(6);

        const float half_width = width/2;

        // Vertices
        vertex_list[0].position = {  half_width , 0 ,  half_width }; //A
        vertex_list[1].position = { -half_width , 0 , -half_width }; //C
        vertex_list[2].position = {  half_width , 0 , -half_width }; //B
        
        vertex_list[3].position = {  half_width , 0 ,  half_width }; //A
        vertex_list[4].position = { -half_width , 0 ,  half_width }; //D
        vertex_list[5].position = { -half_width , 0 , -half_width }; //C

        // Normals
        vertex_list[0].normal = { 0 , 1 , 0 };
        vertex_list[1].normal = { 0 , 1 , 0 };
        vertex_list[2].normal = { 0 , 1 , 0 };

        vertex_list[3].normal = { 0 , 1 , 0 };
        vertex_list[4].normal = { 0 , 1 , 0 };
        vertex_list[5].normal = { 0 , 1 , 0 };

        // Textures
        vertex_list[0].tex_coords = { 0 , 0 };
        vertex_list[1].tex_coords = { 1 , 1 };
        vertex_list[2].tex_coords = { 1 , 0 };

        vertex_list[3].tex_coords = { 0 , 0 };
        vertex_list[4].tex_coords = { 0 , 1 };
        vertex_list[5].tex_coords = { 1 , 1 };

        // Color
        vertex_list[0].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[1].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[2].color = { 0.8f, 0.8f, 0.8f };

        vertex_list[3].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[4].color = { 0.8f, 0.8f, 0.8f };
        vertex_list[5].color = { 0.8f, 0.8f, 0.8f };
        
        return Model(std::move(vertex_list));
    }


    Model torus(const float radius, const float cyl_radius, const uint slices, const uint cyl_slices)
    {
        if(radius <= 0 || cyl_radius <= 0 || cyl_radius >= radius || slices < 3 || cyl_slices < 3)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        const size_t NUM_VTX = 6*slices*cyl_slices;

        std::vector<Vertex> vertex_list(NUM_VTX);
        const float angle = 2*PI / slices;
        const float cyl_angle = 2*PI / cyl_slices;
        const float cyl_distance = radius - cyl_radius;

        const float tex_div_slice = 1.0f / slices;
        const float tex_div_cyl_slice = 1.0f / cyl_slices;

        
        uint it = 0;
        uint it_n = 0;
        uint it_t = 0;

        for(size_t i = 0 ; i < slices ; ++i)
        {
            // Sides
            const float a1 = i*angle;
            const float a2 = (i+1)*angle;

            for(uint j = 0 ; j < cyl_slices ; ++j)
            {
                const float ca1 = j*cyl_angle;
                const float ca2 = (j+1)*cyl_angle;

                // Vertices
                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca1))*std::cos(a1) , cyl_radius*std::sin(ca1) , (cyl_distance + cyl_radius*std::cos(ca1))*std::sin(a1) }; //B
                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca1))*std::cos(a2) , cyl_radius*std::sin(ca1) , (cyl_distance + cyl_radius*std::cos(ca1))*std::sin(a2) }; //A	
                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca2))*std::cos(a1) , cyl_radius*std::sin(ca2) , (cyl_distance + cyl_radius*std::cos(ca2))*std::sin(a1) }; //C

                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca1))*std::cos(a2) , cyl_radius*std::sin(ca1) , (cyl_distance + cyl_radius*std::cos(ca1))*std::sin(a2) }; //A
                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca2))*std::cos(a2) , cyl_radius*std::sin(ca2) , (cyl_distance + cyl_radius*std::cos(ca2))*std::sin(a2) }; //B
                vertex_list[it++].position = { (cyl_distance + cyl_radius*std::cos(ca2))*std::cos(a1) , cyl_radius*std::sin(ca2) , (cyl_distance + cyl_radius*std::cos(ca2))*std::sin(a1) }; //C

                // Normals
                vertex_list[it_n++].normal = { std::cos(ca1)*std::cos(a1) , std::sin(ca1) , std::cos(ca1)*std::sin(a1) }; //B
                vertex_list[it_n++].normal = { std::cos(ca1)*std::cos(a2) , std::sin(ca1) , std::cos(ca1)*std::sin(a2) }; //A	
                vertex_list[it_n++].normal = { std::cos(ca2)*std::cos(a1) , std::sin(ca2) , std::cos(ca2)*std::sin(a1) }; //C

                vertex_list[it_n++].normal = { std::cos(ca1)*std::cos(a2) , std::sin(ca1) , std::cos(ca1)*std::sin(a2) }; //A
                vertex_list[it_n++].normal = { std::cos(ca2)*std::cos(a2) , std::sin(ca2) , std::cos(ca2)*std::sin(a2) }; //B
                vertex_list[it_n++].normal = { std::cos(ca2)*std::cos(a1) , std::sin(ca2) , std::cos(ca2)*std::sin(a1) }; //C

                // Texture_Coords
                vertex_list[it_t++].tex_coords = {     j*tex_div_cyl_slice ,     i*tex_div_slice }; //B
                vertex_list[it_t++].tex_coords = {     j*tex_div_cyl_slice , (i+1)*tex_div_slice }; //A	
                vertex_list[it_t++].tex_coords = { (j+1)*tex_div_cyl_slice ,     i*tex_div_slice }; //C

                vertex_list[it_t++].tex_coords = {     j*tex_div_cyl_slice , (i+1)*tex_div_slice }; //A
                vertex_list[it_t++].tex_coords = { (j+1)*tex_div_cyl_slice , (i+1)*tex_div_slice }; //B
                vertex_list[it_t++].tex_coords = { (j+1)*tex_div_cyl_slice ,     i*tex_div_slice }; //C

                // Texture_Coords
                vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };

                vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
            }
        }

        return Model(std::move(vertex_list));
    }

    
    Model icosahedron(const float radius, const uint divisions, bool to_sphere)
    {
        if(divisions < 1)
        {
            throw std::runtime_error("invalid shape arguments");
        }

        const size_t NUM_VTX = 20*divisions*divisions*3;

        std::vector<Vertex> vertex_list(NUM_VTX);

        uint it = 0;
        uint it_n = 0;
        uint it_t = 0;

        const float cos_30 = cos(PI/6);

        auto compute_emit_tesselation = [&](
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3)
        {
            const float divisions_f = static_cast<float>(divisions);
            const glm::vec3 vector_p1 = (p1 - p2) / divisions_f;
            const glm::vec3 vector_p3 = (p3 - p2) / divisions_f;

            uint iter_divisions = divisions;

            for(std::size_t i = 0; i < divisions; ++i)
            {
                const auto base_pos_i = p2 + static_cast<float>(i)*vector_p1;
                // Vertices
                vertex_list[it++].position = base_pos_i;
                vertex_list[it++].position = base_pos_i + vector_p3;
                vertex_list[it++].position = base_pos_i + vector_p1;

                // Normals
                vertex_list[it_n++].normal = glm::normalize(base_pos_i);
                vertex_list[it_n++].normal = glm::normalize(base_pos_i + vector_p3);
                vertex_list[it_n++].normal = glm::normalize(base_pos_i + vector_p1);

                // Texture Coords
                vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                vertex_list[it_t++].tex_coords = { 0.f, 0.f};

                // Color 
                vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
                
                for(std::size_t j = 1; j < iter_divisions; ++j)
                {
                    const auto base_pos_j = p2 + static_cast<float>(i)*vector_p1 + static_cast<float>(j)*vector_p3;
                    // Vertices
                    vertex_list[it++].position = base_pos_j;
                    vertex_list[it++].position = base_pos_j + vector_p1;
                    vertex_list[it++].position = base_pos_j + vector_p1 - vector_p3;
                    
                    vertex_list[it++].position = base_pos_j;
                    vertex_list[it++].position = base_pos_j + vector_p3;
                    vertex_list[it++].position = base_pos_j + vector_p1;

                    // Normals
                    vertex_list[it_n++].normal = glm::normalize(base_pos_j);
                    vertex_list[it_n++].normal = glm::normalize(base_pos_j + vector_p1);
                    vertex_list[it_n++].normal = glm::normalize(base_pos_j + vector_p1 - vector_p3);

                    vertex_list[it_n++].normal = glm::normalize(base_pos_j);
                    vertex_list[it_n++].normal = glm::normalize(base_pos_j + vector_p3);
                    vertex_list[it_n++].normal = glm::normalize(base_pos_j + vector_p1);

                    // Texture Coords
                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};

                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};
                    vertex_list[it_t++].tex_coords = { 0.f, 0.f};

                    // Color 
                    vertex_list[it_t-6].color = { 0.8f, 0.8f, 0.8f };
                    vertex_list[it_t-5].color = { 0.8f, 0.8f, 0.8f };
                    vertex_list[it_t-4].color = { 0.8f, 0.8f, 0.8f };

                    vertex_list[it_t-3].color = { 0.8f, 0.8f, 0.8f };
                    vertex_list[it_t-2].color = { 0.8f, 0.8f, 0.8f };
                    vertex_list[it_t-1].color = { 0.8f, 0.8f, 0.8f };
                }
                --iter_divisions;
            }
        };

        for(std::size_t i = 0; i < 5; ++i)
        {
            // Points of the connection of the top and the bottom
            // to the body of the icosahedron
            const auto top_1 = cylindrical_to_cartesian({ cos_30*radius, 0.5*radius,     i*(2*PI/5)});
            const auto top_2 = cylindrical_to_cartesian({ cos_30*radius, 0.5*radius, (i+1)*(2*PI/5)});
            const auto bottom_1 = cylindrical_to_cartesian({ cos_30*radius, -0.5*radius,     i*(2*PI/5)+(2*PI/5)/2});
            const auto bottom_2 = cylindrical_to_cartesian({ cos_30*radius, -0.5*radius, (i+1)*(2*PI/5)+(2*PI/5)/2});

            // Vertices
            compute_emit_tesselation({ 0.f, radius, 0.f}, top_2, top_1);
            compute_emit_tesselation({ 0.f, -radius, 0.f}, bottom_1, bottom_2);
            compute_emit_tesselation(top_1, top_2, bottom_1);
            compute_emit_tesselation(top_2, bottom_2, bottom_1);
        }

        if(to_sphere)
        {
            for(auto& vtx : vertex_list)
            {
                vtx.position *= (radius / glm::length(vtx.position));
            }
        }

        return Model(std::move(vertex_list));
    }
} // namespace kzn