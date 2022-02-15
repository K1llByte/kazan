#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_position;
layout(location = 2) in vec3 frag_normal;
layout(location = 3) in vec2 frag_tex;

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

layout(binding = 2) uniform sampler2D tex_sampler;

// Output
layout(location = 0) out vec4 out_color;

//////////////////////////////////////////////////////////

const float shininess = 32.0f;

vec3 compute_dir_light(Light light, vec3 normal, vec3 view_dir, vec3 color)
{
    vec3 light_dir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient  * color; //vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * color; //vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * color; //vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 compute_point_light(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 color)
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
    vec3 ambient  = attenuation * light.ambient  /* * color */;
    vec3 diffuse  = attenuation * light.diffuse  * diff /* * color */;
    vec3 specular = attenuation * light.specular * spec /* * color */;
    return (ambient + diffuse + specular) * color;
}

vec3 compute_lights()
{
    vec3 result = vec3(0.0f);
    const vec3 view_dir = normalize(cam.position - frag_position);
    const vec3 norm_frag_normal = normalize(frag_normal);
    const vec3 color = vec3(texture(tex_sampler, frag_tex));

    for(int i = 0; i < all_lights.used; ++i)
    {
        switch (all_lights.lights[i].type)
        {
            case 0:
                result += compute_dir_light(all_lights.lights[i], norm_frag_normal, view_dir, color);
                break;
            case 1:
                result += compute_point_light(all_lights.lights[i], norm_frag_normal, frag_position, view_dir, color);
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