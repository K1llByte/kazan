struct VertexOut {
    float4 position: SV_Position;
    float2 ndc: TEXCOORD0;
};

// Fullscreen quad
static const float2 positions[4] = {
    float2(-1, -1), float2( 1, -1),
    float2(-1,  1), float2( 1,  1)
};

[shader("vertex")]
VertexOut main(uint vertexIndex: SV_VertexID) {
    float2 pos = positions[vertexIndex];
    
    VertexOut output;
    output.position = float4(pos, 0.0, 1.0);
    output.ndc  = pos;

    return output;
}