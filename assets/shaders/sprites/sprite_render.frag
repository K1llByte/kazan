#version 450

layout(location = 0) in vec2 in_tex_coords;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 0) uniform sampler2D tex_sampler;
layout(set = 1, binding = 1) uniform Sprite {
    // x/y is the offset of the slice in normalized coordinated
    // z/w is the size of the slice in normalized coordinated
    vec4 slice;
    vec4 overlap_color;
} sprite;

void main() {
    vec4 tex_color = texture(
        tex_sampler,
        in_tex_coords * sprite.slice.zw + sprite.slice.xy
    );
    // Blend overlap color over texture sampled color
    out_color = vec4(tex_color.xyz * (1 - sprite.overlap_color.w) + sprite.overlap_color.xyz * sprite.overlap_color.w, tex_color.w);
}