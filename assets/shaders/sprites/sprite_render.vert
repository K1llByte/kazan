#version 450

#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coords;

layout(location = 0) out vec2 out_tex_coords;

layout(set = 0, binding = 0) uniform Camera {
    vec2 position;
    float zoom;
    float aspect_ratio;
    float rotation;
} camera;

layout(push_constant) uniform Pvm {
	mat4 matrix;
} pvm;

void main() {
    // Projection matrix
    mat4 proj_mat = mat4(1.0);
    proj_mat[0].x = (1.0 / camera.aspect_ratio) * camera.zoom;
    proj_mat[1].y = camera.zoom;

    // View matrix
    mat4 view_mat = mat4(1.0);
    view_mat[0].xy = vec2(cos(camera.rotation), sin(camera.rotation));
    view_mat[1].xy = vec2(-sin(camera.rotation), cos(camera.rotation));
    view_mat[3].xy = vec2(-camera.position.x, camera.position.y);

    gl_Position = proj_mat * view_mat * pvm.matrix * vec4(
        in_position.x,
        in_position.y,
        in_position.z,
        1.0
    );

    out_tex_coords = in_tex_coords;
}