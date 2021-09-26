#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_pos;
layout(location = 2) in vec3 frag_normal;

// Uniform buffer block
layout(set = 0, binding = 1) uniform Camera {
    vec3 position;
} cam;


// Output
layout(location = 0) out vec4 out_color;

//////////////////////////////////////////////////////////

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// struct PointLight {
//     vec3 position;

//     float constant;
//     float linear;
//     float quadratic;  

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };


const DirLight dir_light_0 = {
    normalize(vec3(1.0, -3.0, -1.0)),
    vec3(0.02f),
    vec3(1.f),
    vec3(1.f)
};

// const vec3 cam_position = vec3(0.f);

vec3 compute_dir_light(DirLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    // combine results
    vec3 ambient  = light.ambient  * frag_color; //vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * frag_color; //vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * frag_color; //vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

//////////////////////////////////////////////////////////

void main()
{
    vec3 result = compute_dir_light(dir_light_0, frag_normal, cam.position);
    out_color = vec4(result, 1.0);
}