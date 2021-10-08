#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_position;
layout(location = 2) in vec3 frag_normal;

// Uniform buffer block
layout(binding = 0) uniform Camera {
    vec3 position;
} cam;

struct Light {
    uint type;

    // Common
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  
    // Spotlight & PointLight
    vec3 position;
    // Spotlight & DirLight
    vec3 direction;

    // PointLight
    float constant;
    float linear;
    float quadratic;
};

layout(binding = 1) uniform Lights{
    uint used;
    Light lights[8];
} all_lights;


// Output
layout(location = 0) out vec4 out_color;

//////////////////////////////////////////////////////////

// struct DirLight {
//     vec3 direction;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// struct PointLight {
//     vec3 position;

//     float constant;
//     float linear;
//     float quadratic;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

const float shininess = 32.0f;

// const Light dir_light_0 = {
//     0,

//     vec3(0.2f),
//     vec3(0.8f),
//     vec3(1.0f),

//     vec3(0.0f), // null
//     normalize(vec3(1.0, -3.0, -1.0)),

//     0.2f,
//     1.f,
//     0.0f
// };

// const DirLight dir_light_0 = {
//     normalize(vec3(1.0, -3.0, -1.0)),

//     vec3(0.2f),
//     vec3(0.8f),
//     vec3(1.0f)
// };

// const Light point_light_0 = {
//     1,

//     vec3(0.2f),
//     vec3(0.8f),
//     vec3(1.0f),

//     vec3(2.0f),
//     vec3(0.0f), // null

//     0.2f,
//     1.f,
//     0.0f
// };

// const PointLight point_light_0 = {
//     vec3(2.0f, 2.0f, 2.0f),

//     1.0f,
//     1.0f,
//     0.0f,

//     vec3(0.2f),
//     vec3(0.8f),
//     vec3(1.0f)
// };


vec3 compute_dir_light(Light light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient  * frag_color; //vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * frag_color; //vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * frag_color; //vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 compute_point_light(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position - frag_pos);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // attenuation
    float distance    = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  	    light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = attenuation * light.ambient  /* * frag_color */;
    vec3 diffuse  = attenuation * light.diffuse  * diff /* * frag_color */;
    vec3 specular = attenuation * light.specular * spec /* * frag_color */;
    return (ambient + diffuse + specular) * frag_color;
}

vec3 compute_lights()
{
    vec3 result = vec3(0.0f);
    const vec3 view_dir = normalize(cam.position - frag_position);
    const vec3 norm_frag_normal = normalize(frag_normal);
    
    for(int i = 0; i < all_lights.used; ++i)
    {
        switch (all_lights.lights[i].type)
        {
            case 0:
                result += compute_dir_light(all_lights.lights[i], norm_frag_normal, view_dir);
                break;
            case 1:
                result += compute_point_light(all_lights.lights[i], norm_frag_normal, frag_position, view_dir);
                break;
        }
    }

    return result;
}

//////////////////////////////////////////////////////////

void main()
{
    const vec3 view_dir = normalize(cam.position - frag_position);
    // vec3 result = compute_dir_light(dir_light_0, normalize(frag_normal), view_dir);
    // vec3 result = compute_point_light(point_light_0, normalize(frag_normal), frag_position, view_dir);
    vec3 result = compute_lights();
    out_color = vec4(result, 1.0);
}