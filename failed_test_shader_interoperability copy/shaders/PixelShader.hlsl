// PixelShader.hlsl

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Col : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.Col, 1.0f);
}