struct VertexOut {
    float3 color: COLOR0;
    float4 position: SV_Position;
};

[shader("vertex")]
VertexOut vs_main(uint vertex_idx: SV_VertexID) {
    const float2 positions[] = {
        float2( 0.0, -0.5),
        float2( 0.5,  0.5),
        float2(-0.5,  0.5)
    };

    const float3 colors[] = {
        float3(0.984, 0.286, 0.203),
        float3(0.556, 0.752, 0.486),
        float3(0.513, 0.647, 0.596)
    };

    VertexOut output;
    output.position = float4(positions[vertex_idx], 0.0, 1.0);
    output.color = colors[vertex_idx];
    return output;
}

[shader("fragment")]
float4 fs_main(VertexOut input): SV_Target0 {
    return float4(input.color, 1.0);
}