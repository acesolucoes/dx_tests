cbuffer MVP : register(b0)
{
    float4x4 mvp;
};

struct VSInput {
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput output;
    output.pos = mul(mvp, float4(input.pos, 1.0));
    output.col = input.col;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return input.col;
}
