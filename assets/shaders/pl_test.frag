#version 450

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform Camera {
    vec2 position;
    float zoom;
    float aspect_ratio;
    float rotation;
}
camera;

layout(set = 0, binding = 1) uniform Foo {
    vec2 position;
    vec4 rotation;
    vec4 color1;
}
foo;

layout(set = 1, binding = 2) uniform Bar {
    vec4 color2;
}
bar;

void main() {
    out_color = vec4(in_color, camera.zoom) + foo.color1 + bar.color2;
}